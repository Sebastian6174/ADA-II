#pragma once

#include "modelo/tablon.h"

// Voraz mejorado: criterio de urgencia ampliado (incluye rp) + busqueda local
// por intercambio de pares. O(n log n + k*n^3) con k iteraciones de mejora.
// NO garantiza optimalidad, pero suele acercarse mas que el voraz basico.
Respuesta roVM(const Finca& f);
