CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I src
TARGET   = main.exe
BENCH    = benchmark.exe

SRCS = src/main.cpp \
       src/costo/costo.cpp \
       src/algoritmos/fuerza_bruta.cpp \
       src/algoritmos/voraz.cpp \
       src/algoritmos/programacion_dinamica.cpp \
       src/io/archivo_io.cpp \
       src/interfaz/interfaz.cpp

OBJS = $(SRCS:.cpp=.o)

BENCH_OBJS = src/benchmark.o \
             src/costo/costo.o \
             src/algoritmos/fuerza_bruta.o \
             src/algoritmos/voraz.o \
             src/algoritmos/voraz_mejorado.o \
             src/algoritmos/programacion_dinamica.o \
             src/io/archivo_io.o

all: $(TARGET)

benchmark: $(BENCH)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BENCH): $(BENCH_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Recompilar si cambia el limite en el .h
src/benchmark.o: src/benchmark.cpp src/algoritmos/programacion_dinamica.h
src/main.o: src/main.cpp src/algoritmos/programacion_dinamica.h
src/algoritmos/programacion_dinamica.o: src/algoritmos/programacion_dinamica.cpp src/algoritmos/programacion_dinamica.h

run-benchmark: $(BENCH)
	./$(BENCH)

# Windows: py -3 | Linux/WSL: python3
ifeq ($(OS),Windows_NT)
  PYTHON ?= py -3
else
  PYTHON ?= python3
endif

graficas:
	$(PYTHON) scripts/graficas_benchmark.py

benchmark-all: run-benchmark graficas

# PD completa N=30: puede tardar 30+ min
test-n30: $(TARGET)
	@echo "PD N=30: RAM ~12 GiB, CPU muy alto (30+ min posible)..."
	/usr/bin/time -f "Tiempo: %e s | RAM max: %M KiB" ./$(TARGET) datos/entrada/finca_10_limite_30.txt datos/salida/finca_10_limite_30.txt

ifeq ($(OS),Windows_NT)
clean:
	del /S /Q src\*.o 2>nul & del /Q $(TARGET) $(BENCH) $(TEST_ALLOC) 2>nul
else
clean:
	rm -f $(OBJS) $(BENCH_OBJS) $(TARGET) $(BENCH) $(TEST_ALLOC) 2>/dev/null || true
endif

.PHONY: all benchmark run-benchmark graficas benchmark-all test-n30 clean
