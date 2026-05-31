**ADA-II — Sistema de riego óptimo**

Resumen
-------
- **Descripción:** Implementación en C++ de varios algoritmos para el problema de riego óptimo: Fuerza Bruta, Voraz (Greedy) y Programación Dinámica (bitmask).

Requisitos
----------
- **Compilador C++17:** `g++` (MinGW, MSYS, WSL, o GCC/Clang en Linux).
- **make** (opcional, el repositorio incluye `Makefile`).
- **Python 3** (opcional, para generar gráficas con `scripts/graficas_benchmark.py`).

Compilación
-----------
- Usar el `Makefile` (recomendado):

```sh
make        # compila el ejecutable principal (main.exe)
make benchmark    # compila el ejecutable de benchmark (benchmark.exe)
make clean   # limpia objetos y binarios (Windows del.exe usado en Makefile)
```

- Compilación manual (si no dispone de `make`):

```sh
g++ -std=c++17 -Wall -Wextra -O2 -I src \
  -o main.exe \
  src/main.cpp src/costo/costo.cpp src/algoritmos/fuerza_bruta.cpp \
  src/algoritmos/voraz.cpp src/algoritmos/programacion_dinamica.cpp \
  src/io/archivo_io.cpp src/interfaz/interfaz.cpp
```

Ejecución
---------
- Ejecutable principal acepta dos argumentos: `archivo_entrada` `archivo_salida`.
- Si se ejecuta sin argumentos se abre un menú interactivo para seleccionar archivos en `datos/entrada` y `datos/salida`.

Ejemplos (Windows PowerShell / CMD):

```sh
.\main.exe datos/entrada/finca_05_mediana.txt datos/salida/finca_05_mediana.txt
```

Ejemplo en WSL / Linux:

```sh
./main.exe datos/entrada/finca_05_mediana.txt datos/salida/finca_05_mediana.txt
```

Benchmark y gráficas
--------------------
- Compilar el benchmark y ejecutarlo:

```sh
make benchmark
make run-benchmark   # ejecuta benchmark.exe
```

- Generar gráficas (requiere Python 3):

```sh
make graficas        # usa scripts/graficas_benchmark.py
```

Notas y límites
----------------
- El límite para Fuerza Bruta está fijado en `LIMITE_FUERZA_BRUTA = 11` (ver `src/main.cpp`).
- El límite para Programación Dinámica con bitmask está en `LIMITE_PROGRAMACION_DINAMICA = 24` (ver `src/algoritmos/programacion_dinamica.h`). PD puede consumir mucha memoria para N cercano al límite.
- Hay un objetivo de prueba `test-n30` en el `Makefile` que advierte sobre uso intensivo de RAM y tiempo (PD para N=30 puede tardar 30+ minutos y usar mucha memoria).

Estructura relevante
--------------------
- Código fuente: `src/` (algoritmos, IO, interfaz, modelo).
- Entradas de ejemplo: `datos/entrada/`.
- Salidas de ejemplo: `datos/salida/`.
- Script de gráficas: `scripts/graficas_benchmark.py` (requiere `python3` o `py -3`).

Soporte / Sugerencias
---------------------
- En Windows use MinGW/MSYS o WSL para disponer de `make` y `g++` con compatibilidad POSIX.
- Para compilar con Visual Studio, cree un proyecto y agregue los archivos de `src/`.

Licencia
--------
Este repositorio no contiene un archivo de licencia. Añada uno si desea definir los términos de uso.
