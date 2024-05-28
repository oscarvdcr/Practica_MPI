//
// Created by Fernando Cores Prado on 10/4/24.
//

#ifndef CALCCERCA_CALCARBOLESBB_H
#define CALCCERCA_CALCARBOLESBB_H

#include "Solucion.h"
#include "Arbol.h"
#include "CalcCerca.h"


// Prototipos funciones:
int CalcularCercaOptimaBrandBound(PtrSolucionArboles solucion);
int CalcularCercaOptimaBB(int arbol, PtrSolucionArboles solucion);
int CalcularCosteCerca2(PtrSolucionArboles solucion);
PtrSolucionArboles AñadirArbolSolucion(PtrSolucionArboles solucion, int arbol);
void CortarArbolCombinacion(PtrCombinacionArboles combinacion, int arbol);


#endif //CALCCERCA_CALCARBOLESBB_H
