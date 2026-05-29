#pragma once

#include "modelo/tablon.h"

#include <string>

// Muestra los datos de la finca en formato tabla.
void imprimir_finca(const Finca& f);

// Muestra el resultado de un algoritmo (nombre, costo, orden de riego).
void imprimir_respuesta(const string& nombre_algoritmo, const Respuesta& res);

// Lista los archivos en un directorio y permite al usuario elegir uno.
string seleccionar_archivo(const string& directorio, const string& tipo);
