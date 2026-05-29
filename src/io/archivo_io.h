#pragma once

#include "modelo/tablon.h"

#include <string>

// Lee una finca desde un archivo de texto.
// Formato esperado:
//   n
//   ts0 tr0 p0 rp0
//   ts1 tr1 p1 rp1
//   ...
Finca leer_finca(const string& filename);

// Escribe el resultado (costo + orden) en un archivo de texto.
// Formato de salida:
//   Costo
//   pi0
//   pi1
//   ...
void escribir_resultado(const string& filename, const Respuesta& res);
