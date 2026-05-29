#include "algoritmos/fuerza_bruta.h"
#include "costo/costo.h"

#include <algorithm>

using namespace std;

Respuesta roFB(const Finca& f) {
    int n = f.size();
    if (n == 0) return {{}, 0};

    vector<int> orden(n);
    for (int i = 0; i < n; i++) {
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
    } while (next_permutation(orden.begin(), orden.end()));

    return {mejor_orden, costo_minimo};
}

