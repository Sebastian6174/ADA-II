#pragma once

#include "modelo/tablon.h"

// Maximo N para PD por bitmask (O(2^n) memoria).
constexpr int LIMITE_PROGRAMACION_DINAMICA = 24;

// Programacion dinamica con bitmask: explora todos los subconjuntos de
// tablones pendientes usando una mascara de bits como estado.
// Memoizacion sobre S (bitmask) es suficiente porque tiempo_actual
// se deriva deterministicamente de S: es la suma de tr_j para j no en S.
// Complejidad: O(2^n * n) en tiempo, O(2^n) en espacio.
// Siempre optimo (si n <= LIMITE_PROGRAMACION_DINAMICA).
Respuesta roPD(const Finca& f);
