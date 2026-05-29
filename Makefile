CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I src
TARGET   = main.exe

SRCS = src/main.cpp \
       src/costo/costo.cpp \
       src/algoritmos/fuerza_bruta.cpp \
       src/algoritmos/voraz.cpp \
       src/algoritmos/programacion_dinamica.cpp \
       src/io/archivo_io.cpp \
       src/interfaz/interfaz.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	del /S /Q src\*.o 2>nul & del /Q $(TARGET) 2>nul

.PHONY: all clean
