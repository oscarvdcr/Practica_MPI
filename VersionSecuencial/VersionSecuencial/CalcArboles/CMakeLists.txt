cmake_minimum_required(VERSION 3.23)
project(CalcCerca C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS -m64 -lm)

set(SOURCES_FILES CalcArbolesEX.c CalcArbolesBB.c Arbol.c ConvexHull.c Solucion.c Utils.c )
set(HEADER_FILES  CalcArbolesEX.h CalcArbolesBB.h Arbol.h ConvexHull.h Solucion.h Utils.h CalcCerca.h )

add_executable(CalcCerca CalcCerca.c ${SOURCES_FILES} ${HEADER_FILES})
