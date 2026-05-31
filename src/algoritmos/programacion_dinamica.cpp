#include "algoritmos/programacion_dinamica.h"
#include "costo/costo.h"

#include <vector>

using namespace std;

using Mascara = unsigned long long;

static long long tiempo_desde_mascara(Mascara S, int n, const Finca& f) {
    long long t = 0;
    for (int j = 0; j < n; ++j) {
        if (!(S & (1ULL << j))) {
            t += f[j].tr;
        }
    }
    return t;
}

static long long dp_recursivo(Mascara S, int n, const Finca& f,
                              vector<long long>& memo, vector<int>& eleccion) {
    if (S == 0) {
        return 0;
    }

    size_t idx = static_cast<size_t>(S);
    if (memo[idx] != -1) {
        return memo[idx];
    }

    long long tiempo_actual = tiempo_desde_mascara(S, n, f);
    long long costo_min = 2e18;

    for (int i = 0; i < n; ++i) {
        if (S & (1ULL << i)) {
            long long costo = calcular_costo_individual(f[i], tiempo_actual)
                            + dp_recursivo(S ^ (1ULL << i), n, f, memo, eleccion);

            if (costo < costo_min) {
                costo_min = costo;
                eleccion[idx] = i;
            }
        }
    }

    return memo[idx] = costo_min;
}

Respuesta roPD(const Finca& f) {
    int n = static_cast<int>(f.size());
    if (n == 0) {
        return {{}, 0};
    }
    if (n > LIMITE_PROGRAMACION_DINAMICA) {
        return {{}, -1};
    }

    size_t limite = 1ULL << n;
    vector<long long> memo(limite, -1);
    vector<int> eleccion(limite, -1);

    Mascara S_inicial = static_cast<Mascara>(limite - 1);

    long long costo_minimo = dp_recursivo(S_inicial, n, f, memo, eleccion);

    vector<int> orden;
    orden.reserve(n);
    Mascara S = S_inicial;

    while (S > 0) {
        int i = eleccion[static_cast<size_t>(S)];
        orden.push_back(i);
        S ^= (1ULL << i);
    }

    return {orden, costo_minimo};
}
