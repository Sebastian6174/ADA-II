#include "modelo/tablon.h"
#include "io/archivo_io.h"
#include "interfaz/interfaz.h"
#include "algoritmos/fuerza_bruta.h"
#include "algoritmos/voraz.h"
#include "algoritmos/programacion_dinamica.h"

#include <iostream>
#include <string>

using namespace std;

static const int LIMITE_FUERZA_BRUTA = 11;

int main(int argc, char* argv[]) {
    string archivo_entrada;
    string archivo_salida;

    if (argc >= 3) {
        archivo_entrada = argv[1];
        archivo_salida = argv[2];
    } else {
        cout << "==================================================\n";
        cout << "         SISTEMA DE RIEGO OPTIMO (ADA-II)\n";
        cout << "==================================================\n";
        archivo_entrada = seleccionar_archivo("datos/entrada", "ENTRADA");
        
        cout << "\n==================================================\n";
        archivo_salida = seleccionar_archivo("datos/salida", "SALIDA");
        cout << "==================================================\n";
    }

    Finca finca = leer_finca(archivo_entrada);
    if (finca.empty()) {
        cout << "Error: Finca vacia o formato incorrecto. Terminando ejecucion.\n";
        return 1;
    }

    imprimir_finca(finca);
    cout << "\nEjecutando algoritmos...\n";

    // --- Fuerza Bruta ---
    if (static_cast<int>(finca.size()) <= LIMITE_FUERZA_BRUTA) {
        Respuesta resFB = roFB(finca);
        imprimir_respuesta("Fuerza Bruta", resFB);
    } else {
        cout << "\n>> Resultados: Fuerza Bruta <<\n";
        cout << "[Omitido: N=" << finca.size()
             << " es demasiado grande para evaluar O(N!)]\n";
    }

    // --- Voraz ---
    Respuesta resV = roV(finca);
    imprimir_respuesta("Voraz (Greedy)", resV);

    // --- Programacion Dinamica ---
    Respuesta resPD;
    if (static_cast<int>(finca.size()) <= LIMITE_PROGRAMACION_DINAMICA) {
        resPD = roPD(finca);
        imprimir_respuesta("Programacion Dinamica", resPD);
        escribir_resultado(archivo_salida, resPD);

        cout << "\n==================================================\n";
        cout << "[EXITO] Los resultados optimos fueron guardados en '"
             << archivo_salida << "'.\n";
        cout << "==================================================\n\n";
    } else {
        cout << "\n>> Resultados: Programacion Dinamica <<\n";
        cout << "[Omitido: N=" << finca.size()
             << " supera el limite de memoria O(2^N) (max "
             << LIMITE_PROGRAMACION_DINAMICA << " tablones)]\n";
        cout << "\n==================================================\n";
        cout << "[AVISO] No se escribio archivo de salida (PD no ejecutada).\n";
        cout << "==================================================\n\n";
    }

    system("pause");

    return 0;
}
