#include <stdlib.h>
#include <stdio.h>

#include "Solucion.h"
#include "Arbol.h"
#include "CalcArbolesEX.h"
#include "CalcArbolesBB.h"
#include "Utils.h"


int main(int argc, char *argv[])
{
    TSolucionArboles OptimoEX, OptimoBB;

    if (argc<2 || argc>3)
        Error("[Main] Error Argumentos");

    if (!LeerFicheroEntrada(argv[1]))
    {
        Error("[Main] Error lectura fichero entrada.\n");
        exit(1);
    }

    // Si #arboles<32 se calcula la cerca óptima con el método exhaustivo
    if (ArbolesEntrada.NumArboles<32 && !CalcularCercaOptimaExhaustiva(&OptimoEX))
        Error("[Main] Error CalcularCercaOptimaExhaustiva.\n");
    // Se calcula la cerca óptima con el método Branch & Baund con Backtracking
    if (!CalcularCercaOptimaBrandBound(&OptimoBB))
        Error("[Main] Error CalcularCercaOptimaBrandBound.\n");

    // Comprobar que las soluciones de los dos métodos tengan el mismo coste
    if (ArbolesEntrada.NumArboles<32 && OptimoEX.Coste!=OptimoBB.Coste)
        Error("[Main] Error soluciones exahustiva y branch&bound no coinciden.\n");

    // Generar fichero de resultado.
    if (argc==2)
    {
        if (ArbolesEntrada.NumArboles<32 && !GenerarFicheroSalida(OptimoEX, "./Cerca.res"))
            Error("[Main] Error GenerarFicheroSalida.\n");

        if (ArbolesEntrada.NumArboles>=32 && !GenerarFicheroSalida(OptimoBB, "./Cerca.res"))
           Error("[Main] Error GenerarFicheroSalida.\n");
    }
    else
    {
        if (ArbolesEntrada.NumArboles<32 && !GenerarFicheroSalida(OptimoEX, argv[2]))
            Error("[Main] Error Generar fichero salida.\n");

        if (ArbolesEntrada.NumArboles>=32 && !GenerarFicheroSalida(OptimoBB, argv[2]))
            Error("[Main] Error GenerarFicheroSalida.\n");
    }

    exit(0);
}