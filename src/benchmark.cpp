// Benchmark de algoritmos de riego optimo (ADA-II).
//
// Metodologia:
// - Repeticiones con calentamiento (warmup) descartado.
// - En Linux/WSL: cada repeticion corre en un proceso hijo (fork) que termina
//   al acabar, liberando heap y memo de PD; el padre espera (waitpid) y hace
//   una pausa breve entre repeticiones (no garantiza CPU "vacio", pero si
//   aísla memoria entre mediciones).
// - Tiempo: cronometro en el hijo (solo el algoritmo, sin fork del padre).
// - Memoria: getrusage(RUSAGE_SELF).ru_maxrss (KiB en Linux).
// - Sin fork (Windows nativo): mismo proceso + pausa; memoria es pico del
//   proceso acumulado (menos fiable entre repeticiones).
//
// Uso: ./benchmark.exe [directorio_entrada] [repeticiones]
// Salida: consola + datos/salida/benchmark_report.csv

#include "modelo/tablon.h"
#include "io/archivo_io.h"
#include "algoritmos/fuerza_bruta.h"
#include "algoritmos/voraz.h"
#include "algoritmos/voraz_mejorado.h"
#include "algoritmos/programacion_dinamica.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#if defined(__linux__)
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

using namespace std;
namespace fs = filesystem;

static constexpr int LIMITE_FUERZA_BRUTA = 11;
static constexpr int WARMUP_REPETICIONES = 1;
static constexpr int PAUSA_ENTRE_MS = 100;

enum class AlgoritmoId { FuerzaBruta, Voraz, VorazMejorado, ProgramacionDinamica };

struct Estadisticas {
    int n = 0;
    int repeticiones = 0;
    bool ejecutado = false;
    string motivo_omitido;
    long long costo_referencia = 0;
    vector<double> tiempos_ms;
    vector<long> memoria_kb;
};

struct MetricaRun {
    double tiempo_ms = 0.0;
    long memoria_kb = 0;
    long long costo = 0;
    bool ok = false;
};

static void consumir_resultado(const Respuesta& r) {
    volatile long long c = r.second;
    (void)c;
    if (!r.first.empty()) {
        volatile int t = r.first[0];
        (void)t;
    }
}

static void pausa_limpieza() {
#if defined(__linux__)
    sync();
#endif
    this_thread::sleep_for(chrono::milliseconds(PAUSA_ENTRE_MS));
}

static double mediana(vector<double> v) {
    if (v.empty()) return 0.0;
    sort(v.begin(), v.end());
    size_t m = v.size() / 2;
    if (v.size() % 2 == 1) return v[m];
    return (v[m - 1] + v[m]) / 2.0;
}

static double promedio(const vector<double>& v) {
    if (v.empty()) return 0.0;
    double s = 0.0;
    for (double x : v) s += x;
    return s / static_cast<double>(v.size());
}

static long promedio_long(const vector<long>& v) {
    if (v.empty()) return 0;
    long long s = 0;
    for (long x : v) s += x;
    return static_cast<long>(s / static_cast<long long>(v.size()));
}

#if defined(__linux__)
struct ResultadoHijo {
    double tiempo_ms;
    long memoria_kb;
    long long costo;
    int ok;
};

static MetricaRun ejecutar_en_hijo(
    const Finca& finca, AlgoritmoId algo) {
    MetricaRun out;
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        return out;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return out;
    }

    if (pid == 0) {
        close(pipefd[0]);
        struct rusage ru{};
        getrusage(RUSAGE_SELF, &ru);

        auto t0 = chrono::steady_clock::now();
        Respuesta res;
        switch (algo) {
            case AlgoritmoId::FuerzaBruta:
                res = roFB(finca);
                break;
            case AlgoritmoId::Voraz:
                res = roV(finca);
                break;
            case AlgoritmoId::VorazMejorado:
                res = roVM(finca);
                break;
            case AlgoritmoId::ProgramacionDinamica:
                res = roPD(finca);
                break;
        }
        auto t1 = chrono::steady_clock::now();

        getrusage(RUSAGE_SELF, &ru);
        consumir_resultado(res);

        ResultadoHijo msg{};

        msg.tiempo_ms =
            chrono::duration<double, milli>(t1 - t0).count();
        msg.memoria_kb = ru.ru_maxrss;
        msg.costo = res.second;
        msg.ok = 1;

        ssize_t w = write(pipefd[1], &msg, sizeof(msg));
        (void)w;
        close(pipefd[1]);
        _exit(0);
    }

    close(pipefd[1]);

    ResultadoHijo msg{};
    ssize_t r = read(pipefd[0], &msg, sizeof(msg));
    close(pipefd[0]);

    int status = 0;
    waitpid(pid, &status, 0);

    if (r == static_cast<ssize_t>(sizeof(msg)) && WIFEXITED(status)
        && WEXITSTATUS(status) == 0 && msg.ok) {
        out.tiempo_ms = msg.tiempo_ms;
        out.memoria_kb = msg.memoria_kb;
        out.costo = msg.costo;
        out.ok = true;
    }

    pausa_limpieza();
    return out;
}
#else
static MetricaRun ejecutar_en_proceso(
    const Finca& finca, AlgoritmoId algo) {
    MetricaRun out;
    auto t0 = chrono::steady_clock::now();
    Respuesta res;
    switch (algo) {
        case AlgoritmoId::FuerzaBruta:
            res = roFB(finca);
            break;
        case AlgoritmoId::Voraz:
            res = roV(finca);
            break;
        case AlgoritmoId::VorazMejorado:
            res = roVM(finca);
            break;
        case AlgoritmoId::ProgramacionDinamica:
            res = roPD(finca);
            break;
    }
    auto t1 = chrono::steady_clock::now();
    consumir_resultado(res);

    out.tiempo_ms = chrono::duration<double, milli>(t1 - t0).count();
    out.memoria_kb = -1;
    out.costo = res.second;
    out.ok = true;
    pausa_limpieza();
    return out;
}
#endif

static MetricaRun ejecutar_medicion(
    const Finca& finca, AlgoritmoId algo) {
#if defined(__linux__)
    return ejecutar_en_hijo(finca, algo);
#else
    return ejecutar_en_proceso(finca, algo);
#endif
}

static bool puede_ejecutar(AlgoritmoId algo, int n, string& motivo) {
    switch (algo) {
        case AlgoritmoId::FuerzaBruta:
            if (n > LIMITE_FUERZA_BRUTA) {
                motivo = "N>" + to_string(LIMITE_FUERZA_BRUTA) + " O(N!)";
                return false;
            }
            return true;
        case AlgoritmoId::Voraz:
            return true;
        case AlgoritmoId::VorazMejorado:
            return true;
        case AlgoritmoId::ProgramacionDinamica:
            if (n > LIMITE_PROGRAMACION_DINAMICA) {
                motivo = "N>" + to_string(LIMITE_PROGRAMACION_DINAMICA)
                         + " O(2^N) RAM";
                return false;
            }
            return true;
    }
    return false;
}

static string nombre_algoritmo(AlgoritmoId algo) {
    switch (algo) {
        case AlgoritmoId::FuerzaBruta: return "FuerzaBruta";
        case AlgoritmoId::Voraz: return "Voraz";
        case AlgoritmoId::VorazMejorado: return "VorazMejorado";
        case AlgoritmoId::ProgramacionDinamica: return "ProgramacionDinamica";
    }
    return "?";
}

static Estadisticas bench_algoritmo(
    const Finca& finca, AlgoritmoId algo, int repeticiones) {
    Estadisticas st;
    st.n = static_cast<int>(finca.size());
    st.repeticiones = repeticiones;

    string motivo;
    if (!puede_ejecutar(algo, st.n, motivo)) {
        st.ejecutado = false;
        st.motivo_omitido = motivo;
        return st;
    }

    for (int w = 0; w < WARMUP_REPETICIONES; ++w) {
        ejecutar_medicion(finca, algo);
    }

    st.tiempos_ms.reserve(repeticiones);
    st.memoria_kb.reserve(repeticiones);

    for (int r = 0; r < repeticiones; ++r) {
        MetricaRun m = ejecutar_medicion(finca, algo);
        if (!m.ok) continue;
        st.tiempos_ms.push_back(m.tiempo_ms);
        st.memoria_kb.push_back(m.memoria_kb);
        st.costo_referencia = m.costo;
    }

    st.ejecutado = !st.tiempos_ms.empty();
    if (!st.ejecutado) {
        st.motivo_omitido = "fallo en medicion";
    }
    return st;
}

static vector<string> listar_fincas(const string& dir) {
    vector<string> archivos;
    error_code ec;
    if (!fs::is_directory(dir, ec)) {
        return archivos;
    }
    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (!entry.is_regular_file()) continue;
        string nombre = entry.path().filename().string();
        if (nombre.size() >= 6 && nombre.compare(0, 6, "finca_") == 0
            && nombre.find(".txt") != string::npos) {
            archivos.push_back(entry.path().string());
        }
    }
    sort(archivos.begin(), archivos.end());
    return archivos;
}

static void imprimir_estadisticas(
    const string& finca_nombre, const Estadisticas& st, AlgoritmoId algo) {
    cout << left << setw(28) << finca_nombre
         << setw(22) << nombre_algoritmo(algo)
         << "N=" << setw(3) << st.n << " ";

    if (!st.ejecutado) {
        cout << "[OMITIDO: " << st.motivo_omitido << "]\n";
        return;
    }

    vector<double> t = st.tiempos_ms;
    double t_min = *min_element(t.begin(), t.end());
    double t_max = *max_element(t.begin(), t.end());
    double t_med = mediana(t);
    double t_avg = promedio(t);

    cout << "rep=" << st.repeticiones
         << "  t_min=" << fixed << setprecision(2) << t_min << "ms"
         << "  t_med=" << t_med << "ms"
         << "  t_avg=" << t_avg << "ms"
         << "  t_max=" << t_max << "ms";

    if (!st.memoria_kb.empty() && st.memoria_kb[0] >= 0) {
        long m_min = *min_element(st.memoria_kb.begin(), st.memoria_kb.end());
        long m_max = *max_element(st.memoria_kb.begin(), st.memoria_kb.end());
        long m_avg = promedio_long(st.memoria_kb);
        cout << "  RSS_max=" << m_min << "/" << m_avg << "/" << m_max << " KiB";
    } else {
        cout << "  RSS=N/D";
    }

    cout << "  costo=" << st.costo_referencia << "\n";
}

static void escribir_csv(
    ofstream& csv,
    const string& finca_nombre,
    const Estadisticas& st,
    AlgoritmoId algo) {
    csv << finca_nombre << ","
        << st.n << ","
        << nombre_algoritmo(algo) << ","
        << st.repeticiones << ",";

    if (!st.ejecutado) {
        csv << ",,,,,,,," << st.motivo_omitido << "\n";
        return;
    }

    vector<double> t = st.tiempos_ms;
    double t_min = *min_element(t.begin(), t.end());
    double t_max = *max_element(t.begin(), t.end());

    long m_min = -1, m_max = -1, m_avg = -1;
    if (!st.memoria_kb.empty() && st.memoria_kb[0] >= 0) {
        m_min = *min_element(st.memoria_kb.begin(), st.memoria_kb.end());
        m_max = *max_element(st.memoria_kb.begin(), st.memoria_kb.end());
        m_avg = promedio_long(st.memoria_kb);
    }

    csv << fixed << setprecision(3)
        << t_min << ","
        << mediana(t) << ","
        << promedio(t) << ","
        << t_max << ","
        << m_min << ","
        << m_avg << ","
        << m_max << ","
        << st.costo_referencia << ",\n";
}

int main(int argc, char* argv[]) {
    string dir_entrada = "datos/entrada";
    int repeticiones = 5;

    if (argc >= 2) dir_entrada = argv[1];
    if (argc >= 3) repeticiones = max(1, atoi(argv[2]));

    vector<string> fincas = listar_fincas(dir_entrada);
    if (fincas.empty()) {
        cerr << "No se encontraron archivos finca_*.txt en " << dir_entrada
             << "\n";
        return 1;
    }

    fs::create_directories("datos/salida");
    string reporte_path = "datos/salida/benchmark_report.csv";
    ofstream csv(reporte_path);
    if (!csv) {
        cerr << "No se pudo crear " << reporte_path << "\n";
        return 1;
    }

    cout << "============================================================\n";
    cout << "  BENCHMARK ADA-II - Riego optimo\n";
    cout << "============================================================\n";
    cout << "Directorio: " << dir_entrada << "\n";
    cout << "Repeticiones medidas: " << repeticiones
         << " (+ " << WARMUP_REPETICIONES << " warmup descartado)\n";
    cout << "Pausa entre runs: " << PAUSA_ENTRE_MS << " ms\n";
#if defined(__linux__)
    cout << "Aislamiento: fork() por repeticion (Linux/WSL)\n";
    cout << "Memoria: getrusage().ru_maxrss (KiB, pico del hijo)\n";
#else
    cout << "Aislamiento: mismo proceso (memoria RSS no disponible)\n";
#endif
    cout << "Reporte CSV: " << reporte_path << "\n";
    cout << "============================================================\n\n";

    csv << "finca,N,algoritmo,repeticiones,"
           "tiempo_min_ms,tiempo_mediana_ms,tiempo_prom_ms,tiempo_max_ms,"
           "mem_min_kb,mem_prom_kb,mem_max_kb,costo,notas\n";

    const AlgoritmoId algoritmos[] = {
        AlgoritmoId::FuerzaBruta,
        AlgoritmoId::Voraz,
        AlgoritmoId::VorazMejorado,
        AlgoritmoId::ProgramacionDinamica
    };

    for (const string& ruta : fincas) {
        Finca finca = leer_finca(ruta);
        if (finca.empty()) {
            cerr << "Omitiendo (lectura fallida): " << ruta << "\n";
            continue;
        }

        string nombre = fs::path(ruta).filename().string();
        cout << "--- " << nombre << " ---\n";

        Estadisticas ref_fb{};
        bool tiene_ref = false;

        for (AlgoritmoId algo : algoritmos) {
            Estadisticas st = bench_algoritmo(finca, algo, repeticiones);
            imprimir_estadisticas(nombre, st, algo);
            escribir_csv(csv, nombre, st, algo);

            if (algo == AlgoritmoId::FuerzaBruta && st.ejecutado) {
                ref_fb = st;
                tiene_ref = true;
            } else if (tiene_ref && st.ejecutado
                       && st.costo_referencia != ref_fb.costo_referencia) {
                cout << "  [AVISO] costo difiere de FuerzaBruta ("
                     << st.costo_referencia << " vs " << ref_fb.costo_referencia
                     << ")\n";
            }
        }
        cout << "\n";
        finca.clear();
        finca.shrink_to_fit();
        pausa_limpieza();
    }

    cout << "============================================================\n";
    cout << "Benchmark finalizado. CSV: " << reporte_path << "\n";
    cout << "Tip: perfil detallado con: perf stat -d ./benchmark.exe\n";
    cout << "     o: /usr/bin/time -v ./benchmark.exe\n";
    cout << "============================================================\n";

    return 0;
}
