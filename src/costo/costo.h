#pragma once

#include "modelo/tablon.h"

// Calcula el costo de riego de un tablon individual.
// t: instante en que se inicia el riego del tablon.
// Casos (el caso 1 tiene prioridad sobre los demas):
//   1. t == rp  ->  ts - (t + tr)            (riego perfecto)
//   2. ts - tr >= t  ->  2 * (ts - (t + tr))  (a tiempo)
//   3. otro caso  ->  2 * p * ((t + tr) - ts) (tarde)
long long calcular_costo_individual(const Tablon& T, long long t);

// Calcula el costo total de riego de una finca dado un orden (permutacion).
long long calcular_costo_total(const Finca& f, const vector<int>& orden);
