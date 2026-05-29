#pragma once

#include <vector>
#include <utility>

using namespace std;

// Unidad basica de cultivo en una finca
struct Tablon {
    long long ts; // tiempo de supervivencia
    long long tr; // tiempo de regado
    int p;        // prioridad (1..4, siendo 4 la mas alta)
    long long rp; // tiempo de riego perfecto
    int id;       // indice original del tablon
};

// Una finca es una secuencia de tablones
using Finca = vector<Tablon>;

// Resultado: (permutacion de riego, costo total)
using Respuesta = pair<vector<int>, long long>;
