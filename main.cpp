#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <iomanip>
#include <climits>

using namespace std;

// 1. TIPOS DE DATOS Y CONTRATO DE INTERFAZ

struct Tablon {
    long long ts; // tiempo de supervivencia
    long long tr; // tiempo de regado
    int p;        // prioridad
    long long rp; // tiempo de riego perfecto
    int id;       // ID original para saber qué tablón es
};

using Finca = vector<Tablon>;
using Respuesta = pair<vector<int>, long long>;

Respuesta roFB(const Finca& f);
Respuesta roV(const Finca& f);
Respuesta roPD(const Finca& f);

// 2. FUNCIONES DE COSTO

long long calcular_costo_individual(const Tablon& T, long long t) {
    if (t == T.rp) {
        return T.ts - (t + T.tr);
    } else if (T.ts - T.tr >= t) {
        return 2LL * (T.ts - (t + T.tr));
    } else {
        return 2LL * T.p * ((t + T.tr) - T.ts);
    }
}

long long calcular_costo_total(const Finca& f, const vector<int>& orden) {
    long long costo_total = 0;
    long long tiempo_actual = 0;
    for (int i : orden) {
        costo_total += calcular_costo_individual(f[i], tiempo_actual);
        tiempo_actual += f[i].tr;
    }
    return costo_total;
}

// 3. ENTRADA Y SALIDA DE ARCHIVOS

Finca leer_finca(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Error: No se pudo abrir el archivo de entrada '" << filename << "'\n";
        return {};
    }
    int n;
    if (!(in >> n)) return {};
    
    Finca f(n);
    for (int i = 0; i < n; ++i) {
        in >> f[i].ts >> f[i].tr >> f[i].p >> f[i].rp;
        f[i].id = i;
    }
    return f;
}

void escribir_resultado(const string& filename, const Respuesta& res) {
    ofstream out(filename);
    if (!out) {
        cerr << "Error: No se pudo abrir el archivo de salida '" << filename << "'\n";
        return;
    }
    out << res.second << "\n";
    for (int idx : res.first) {
        out << idx << "\n";
    }
}

// 4. ALGORITMO: FUERZA BRUTA

Respuesta roFB(const Finca& f) {
    int n = f.size();
    if (n == 0) return {{}, 0};
    vector<int> orden(n);

    for (int i = 0; i < n; i++){
     orden[i] = i;
    }
   
    vector<int> mejor_orden = orden;
    long long costo_minimo = 2e18;

    do {
        long long costo_actual = calcular_costo_total(f, orden);
        if (costo_actual < costo_minimo) {
            costo_minimo = costo_actual;
            mejor_orden = orden;
        }
    } while (next_permutation(orden.begin(), orden.end())); // hay que verificar si permiten next_permutation de la libreria <algorithm>
    return {mejor_orden, costo_minimo};
}


// 5. ALGORITMO: VORAZ 
// Heurística: Menor costo local. 
// Desempate 1: Mayor prioridad (p).
// Desempate 2: Menor tiempo de supervivencia (ts).

Respuesta roV(const Finca& f) {
    int n = f.size();
    if (n == 0) return {{}, 0};

    vector<int> orden;
    vector<bool> regado(n, false);
    long long tiempo_actual = 0;
    long long costo_total = 0;

    for (int paso = 0; paso < n; ++paso) {
        int mejor_candidato = -1;
        long long menor_costo_local = 2e18;
        
        for (int i = 0; i < n; ++i) {
            if (!regado[i]) {
                long long costo_local = calcular_costo_individual(f[i], tiempo_actual);
                if (mejor_candidato == -1 || costo_local < menor_costo_local) {
                    mejor_candidato = i;
                    menor_costo_local = costo_local;
                } else if (costo_local == menor_costo_local) {
                    if (f[i].p > f[mejor_candidato].p) {
                        mejor_candidato = i;
                    } else if (f[i].p == f[mejor_candidato].p) {
                        if (f[i].ts < f[mejor_candidato].ts) {
                            mejor_candidato = i;
                        }
                    }
                }
            }
        }
        
        orden.push_back(mejor_candidato);
        regado[mejor_candidato] = true;
        costo_total += menor_costo_local;
        tiempo_actual += f[mejor_candidato].tr;
    }

    return {orden, costo_total};
}

// 6. ALGORITMO: PROGRAMACIÓN DINÁMICA

Respuesta roPD(const Finca& f) {
    int n = f.size();
    if (n == 0) return {{}, 0};

    int limite = 1 << n;
    vector<long long> memo(limite, -1);
    vector<int> eleccion(limite, -1);

    auto dp = [&](auto& self, int S, long long tiempo_actual) -> long long {
        if (S == 0) return 0;
        if (memo[S] != -1) return memo[S];

        long long costo_min = 2e18;
        for (int i = 0; i < n; ++i) {
            if (S & (1 << i)) { // Si el tablón i está disponible (en S)
                long long costo = calcular_costo_individual(f[i], tiempo_actual) 
                                + self(self, S ^ (1 << i), tiempo_actual + f[i].tr);
                
                if (costo < costo_min) {
                    costo_min = costo;
                    eleccion[S] = i;
                }
            }
        }
        return memo[S] = costo_min;
    };

    // S_inicial tiene encendidos los primeros n bits
    int S_inicial = (1 << n) - 1;
    long long costo_minimo = dp(dp, S_inicial, 0);

    // Reconstruir la solución
    vector<int> orden;
    int S = S_inicial;
    while (S > 0) {
        int i = eleccion[S];
        orden.push_back(i);
        S ^= (1 << i);
    }

    return {orden, costo_minimo};
}

// 7. INTERFAZ VISUAL (CONSOLA) Y MAIN

void imprimir_finca(const Finca& f) {
    cout << "\n==================================================\n";
    cout << "              DATOS DE LA FINCA\n";
    cout << "==================================================\n";
    cout << left << setw(10) << "Tablon" << setw(10) << "ts" << setw(10) << "tr" << setw(10) << "p" << setw(10) << "rp" << "\n";
    cout << "--------------------------------------------------\n";
    for (int i = 0; i < f.size(); ++i) {
        cout << left << setw(10) << i 
             << setw(10) << f[i].ts 
             << setw(10) << f[i].tr 
             << setw(10) << f[i].p 
             << setw(10) << f[i].rp << "\n";
    }
    cout << "==================================================\n";
}

void imprimir_respuesta(const string& nombre_algoritmo, const Respuesta& res) {
    cout << "\n>> Resultados: " << nombre_algoritmo << " <<\n";
    cout << "Costo Total: " << res.second << "\n";
    cout << "Orden de riego: ";
    for (int i = 0; i < res.first.size(); ++i) {
        cout << res.first[i] << (i + 1 == res.first.size() ? "" : " -> ");
    }
    cout << "\n";
}

int main(int argc, char* argv[]) {
    string archivo_entrada, archivo_salida;
    if (argc >= 3) {
        archivo_entrada = argv[1];
        archivo_salida = argv[2];
    } else {
        cout << "==================================================\n";
        cout << "         SISTEMA DE RIEGO OPTIMO (ADA-II)\n";
        cout << "==================================================\n\n";
        cout << "Ingrese el nombre del archivo de ENTRADA (ej: in.txt): ";
        cin >> archivo_entrada;
        cout << "Ingrese el nombre del archivo de SALIDA (ej: out.txt): ";
        cin >> archivo_salida;
    }

    Finca finca = leer_finca(archivo_entrada);
    if (finca.empty()) {
        cout << "Error: Finca vacia o formato incorrecto. Terminando ejecucion.\n";
        return 1;
    }

    imprimir_finca(finca);

    cout << "\nEjecutando algoritmos...\n";
    if (finca.size() <= 11) {
        Respuesta resFB = roFB(finca);
        imprimir_respuesta("Fuerza Bruta", resFB);
    } else {
        cout << "\n>> Resultados: Fuerza Bruta <<\n";
        cout << "[Omitido: N=" << finca.size() << " es demasiado grande para evaluar O(N!)]\n";
    }

    // Ejecutar Voraz
    Respuesta resV = roV(finca);
    imprimir_respuesta("Voraz (Greedy)", resV);

    // Ejecutar Programación Dinámica
    Respuesta resPD = roPD(finca);
    imprimir_respuesta("Programacion Dinamica", resPD);
    escribir_resultado(archivo_salida, resPD);
    cout << "\n==================================================\n";
    cout << "[EXITO] Los resultados optimos fueron guardados en '" << archivo_salida << "'.\n";
    cout << "==================================================\n";

    return 0;
}