#include "algoritmos/programacion_dinamica.h"
#include "costo/costo.h"

#include <vector>

using namespace std;

// Funcion recursiva tradicional auxiliar
long long dp_recursivo(int S, long long tiempo_actual, int n, const Finca& f, 
                       vector<long long>& memo, vector<int>& eleccion) {
    
    
    if (S == 0) {
        return 0;
    }
    if (memo[S] != -1) {
        return memo[S];
    }

    long long costo_min = 2e18;

    for (int i = 0; i < n; ++i) {
        if (S & (1 << i)) {
            long long costo = calcular_costo_individual(f[i], tiempo_actual)
                            + dp_recursivo(S ^ (1 << i), tiempo_actual + f[i].tr, n, f, memo, eleccion);

            if (costo < costo_min) {
                costo_min = costo;
                eleccion[S] = i;
            }
        }
    }

    return memo[S] = costo_min;
}

Respuesta roPD(const Finca& f) {
    int n = f.size();
    if (n == 0) return {{}, 0};

    // OPCION 1: Top-Down con Memoizacion
    
    int limite = 1 << n;
    vector<long long> memo(limite, -1);
    vector<int> eleccion(limite, -1);

    int S_inicial = (1 << n) - 1;
    
    // Llamada a la funcion auxiliar pasando estado por referencia
    long long costo_minimo = dp_recursivo(S_inicial, 0, n, f, memo, eleccion);

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

  
  
    // OPCION 2: Bottom-Up Iterativa
    
    /* 
    int limite = 1 << n;
    vector<long long> memo(limite, 2e18); // Inicializado con "infinito"
    vector<int> eleccion(limite, -1);

    memo[0] = 0;

    for (int S = 0; S < limite; S++) {
        if (memo[S] == 2e18) continue;

        // Calcular el tiempo que ha pasado para regar los tablones que YA estan en S
        long long tiempo_actual = 0;
        for (int i = 0; i < n; i++) {
            bool tablon_ya_regado = (S & (1 << i)) != 0;
            if (tablon_ya_regado) {
                tiempo_actual += f[i].tr;
            }
        }

        // Transiciones: Agregar un nuevo tablon i que todavia no esta en S
        for (int i = 0; i < n; i++) {
            bool tablon_ya_regado = (S & (1 << i)) != 0;
            
            if (!tablon_ya_regado) { 
                int next_S = S | (1 << i);
                long long costo = calcular_costo_individual(f[i], tiempo_actual) + memo[S];
                
                if (costo < memo[next_S]) {
                    memo[next_S] = costo;
                    eleccion[next_S] = i; 
                }
            }
        }
    }

    long long costo_minimo = memo[limite - 1];

    // Reconstruir la solucion iterando al reves
    vector<int> orden(n);
    int S = limite - 1;
    for (int step = n - 1; step >= 0; step--) {
        int i = eleccion[S];
        orden[step] = i;
        S ^= (1 << i); 
    }

    return {orden, costo_minimo};
    */
}
