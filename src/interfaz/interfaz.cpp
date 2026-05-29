#include "interfaz/interfaz.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <dirent.h>

using namespace std;

void imprimir_finca(const Finca& f) {
    cout << "\n==================================================\n";
    cout << "              DATOS DE LA FINCA\n";
    cout << "==================================================\n";
    cout << left
         << setw(10) << "Tablon"
         << setw(10) << "ts"
         << setw(10) << "tr"
         << setw(10) << "p"
         << setw(10) << "rp" << "\n";
    cout << "--------------------------------------------------\n";

    for (size_t i = 0; i < f.size(); ++i) {
        cout << left
             << setw(10) << i
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

    for (size_t i = 0; i < res.first.size(); ++i) {
        cout << res.first[i];
        if (i + 1 < res.first.size()) cout << " -> ";
    }

    cout << "\n";
}

string seleccionar_archivo(const string& directorio, const string& tipo) {
    vector<string> archivos;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directorio.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string nombre = ent->d_name;
            if (nombre != "." && nombre != "..") {
                archivos.push_back(nombre);
            }
        }
        closedir(dir);
    }

    if (archivos.empty()) {
        cout << "No se encontraron archivos en " << directorio << "\n";
        cout << "Ingrese el nombre del archivo de " << tipo << " manualmente: ";
        string manual;
        cin >> manual;
        return directorio + "/" + manual;
    }

    cout << "\nArchivos disponibles en " << directorio << ":\n";
    for (size_t i = 0; i < archivos.size(); ++i) {
        cout << "  " << i + 1 << ". " << archivos[i] << "\n";
    }
    cout << "  " << archivos.size() + 1 << ". [Ingresar nombre manualmente]\n";

    int opcion = -1;
    while (true) {
        cout << "Seleccione una opcion (1-" << archivos.size() + 1 << "): ";
        cin >> opcion;
        if (cin.fail() || opcion < 1 || opcion > static_cast<int>(archivos.size()) + 1) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Opcion invalida. Intente de nuevo.\n";
        } else {
            break;
        }
    }

    if (opcion == static_cast<int>(archivos.size()) + 1) {
        cout << "Ingrese el nombre del archivo de " << tipo << " manualmente: ";
        string manual;
        cin >> manual;
        return directorio + "/" + manual;
    }

    return directorio + "/" + archivos[opcion - 1];
}
