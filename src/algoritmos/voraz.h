#pragma once

#include "modelo/tablon.h"

// Voraz: ordena por urgencia ponderada (WSJF).
// Criterio: (ts - tr) / p ascendente (implementado con producto cruzado
// para evitar divisiones de punto flotante).
// Complejidad: O(n log n). NO garantiza optimalidad.
Respuesta roV(const Finca& f);
