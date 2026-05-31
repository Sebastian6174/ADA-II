#include "algoritmos/voraz_mejorado.h"
#include "costo/costo.h"

#include <algorithm>
#include <vector>

using namespace std;

static bool compararTablonesMejorado(const Tablon& a, const Tablon& b) {
    double urgenciaA = static_cast<double>(a.ts - a.tr) / a.p;
    double urgenciaB = static_cast<double>(b.ts - b.tr) / b.p;

    if (urgenciaA != urgenciaB) {
        return urgenciaA < urgenciaB;
    }
    if (a.p != b.p) {
        return a.p > b.p;
    }
    if (a.rp != b.rp) {
        return a.rp < b.rp;
    }
    return a.ts < b.ts;
}

static void busquedaLocal(const Finca& f, vector<int>& indices, long long& costo) {
    int n = static_cast<int>(indices.size());
    bool mejoro = true;

    while (mejoro) {
        mejoro = false;
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                swap(indices[i], indices[j]);
                long long nuevo_costo = calcular_costo_total(f, indices);
                if (nuevo_costo < costo) {
                    costo = nuevo_costo;
                    mejoro = true;
                } else {
                    swap(indices[i], indices[j]);
                }
            }
        }
    }
}

Respuesta roVM(const Finca& f) {
    int n = static_cast<int>(f.size());
    if (n == 0) {
        return {{}, 0};
    }

    vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = i;
    }

    sort(indices.begin(), indices.end(), [&f](int i, int j) {
        return compararTablonesMejorado(f[i], f[j]);
    });

    long long costo = calcular_costo_total(f, indices);
    busquedaLocal(f, indices, costo);

    vector<int> orden;
    orden.reserve(n);
    for (int idx : indices) {
        orden.push_back(f[idx].id);
    }

    return {orden, costo};
}
