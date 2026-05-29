#include "algoritmos/voraz.h"
#include "costo/costo.h"

#include <algorithm>
#include <vector>

using namespace std;

bool compararTablones(const Tablon& a, const Tablon& b) {
    double urgenciaA = static_cast<double>(a.ts - a.tr) / a.p;
    double urgenciaB = static_cast<double>(b.ts - b.tr) / b.p;

    if (urgenciaA != urgenciaB) {
        return urgenciaA < urgenciaB;
    }
    if (a.p != b.p) {
        return a.p > b.p; // Desempate 1
    }
    return a.ts < b.ts;   // Desempate 2
}

Respuesta roV(const Finca& f) {
    int n = f.size(); // Total de tablones
    
    if (n == 0){
        return {{}, 0}; // Finca vacía, devuelve vacío
    } 

    // Nota para Juli: Lo que hace es que crea un vector con los indices, para representar 
    // numericamente a cada tablón y poder ordenarlos segun la funcion compararTablones
    vector<int> indices(n);
    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }

    sort(indices.begin(), indices.end(), [&f](int i, int j) {
        return compararTablones(f[i], f[j]);
    });

    vector<int> orden;
    orden.reserve(n);
    long long tiempo_actual = 0;
    long long costo_total = 0;

    for (int k = 0; k < n; k++) {
        int idx = indices[k];
        orden.push_back(f[idx].id);
        costo_total += calcular_costo_individual(f[idx], tiempo_actual);
        tiempo_actual += f[idx].tr;
    }

    return {orden, costo_total};
}
