#include "io/archivo_io.h"

#include <fstream>
#include <iostream>

using namespace std;

Finca leer_finca(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Error: No se pudo abrir el archivo de entrada '"
             << filename << "'" << endl;
        return {};
    }

    int n;
    if (!(in >> n)) return {};

    Finca f(n);
    for (int i = 0; i < n; ++i) {
        char comma1, comma2, comma3;
        in >> f[i].ts >> comma1 >> f[i].tr >> comma2 >> f[i].p >> comma3 >> f[i].rp;
        f[i].id = i;
    }

    return f;
}

void escribir_resultado(const string& filename, const Respuesta& res) {
    ofstream out(filename);
    if (!out) {
        cerr << "Error: No se pudo abrir el archivo de salida '"
             << filename << "'" << endl;
        return;
    }

    out << res.second << "\n";
    for (int idx : res.first) {
        out << idx << "\n";
    }
}
