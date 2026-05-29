#pragma once

#include "modelo/tablon.h"

// Fuerza bruta: genera todas las permutaciones posibles y devuelve la de
// menor costo. Complejidad: O(n! * n). Siempre optimo por busqueda exhaustiva.
Respuesta roFB(const Finca& f);
