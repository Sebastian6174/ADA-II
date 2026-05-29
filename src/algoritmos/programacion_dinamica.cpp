#include "algoritmos/programacion_dinamica.h"
#include "costo/costo.h"

#include <vector>

using namespace std;

Respuesta roPD(const Finca& f) {
    int n = f.size();
    if (n == 0) return {{}, 0};

    int limite = 1 << n;
    vector<long long> memo(limite, -1);
    vector<int> eleccion(limite, -1);

    // dp(S, tiempo_actual) -> costo minimo para regar los tablones en S.
    // tiempo_actual se pasa por conveniencia; es deterministico dado S.
    auto dp = [&](auto& self, int S, long long tiempo_actual) -> long long {
        if (S == 0) return 0;
        if (memo[S] != -1) return memo[S];

        long long costo_min = 2e18;

        for (int i = 0; i < n; ++i) {
            if (S & (1 << i)) {
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

    int S_inicial = (1 << n) - 1;
    long long costo_minimo = dp(dp, S_inicial, 0);

    // Reconstruir la solucion optima
    vector<int> orden;
    orden.reserve(n);
    int S = S_inicial;

    while (S > 0) {
        int i = eleccion[S];
        orden.push_back(i);
        S ^= (1 << i);
    }

    return {orden, costo_minimo};
}
