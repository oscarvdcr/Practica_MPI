//
// Created by Fernando Cores Prado on 10/4/24.
//

#ifndef CALCCERCA_SOLUCION_H
#define CALCCERCA_SOLUCION_H

#include "Arbol.h"


#define DMaximoCoste INT_MAX


// Solución.
struct SolucionArboles
{
    TCombinacionArboles Combinacion;
    TListaArboles       ArbolesTalados;         // Arboles Talados.
    int		            Coste;                  // Coste de los árboles cortados.
    int		            CosteArbolesRestantes;  // Coste de los árboles cercados.
    float		        LongitudCerca;          // Longitud cerca para rodear arboles que quedan.
    float		        MaderaSobrante;         // Madera que sobra de la cerca.
    //int 		        NumArboles;             // Número árboles cortados.
    //int 		        Arboles[DMaxArboles];   // Arboles Talados.
};
typedef struct SolucionArboles TSolucionArboles, *PtrSolucionArboles;


// Prototipoes Funciones:
bool GenerarFicheroSalida(TSolucionArboles optimo, char *PathFicOut);
//void MostrarArboles(TSolucionArboles CombinacionArboles);
//void MostrarInfoArboles(TSolucionArboles CombinacionArboles);

#endif //CALCCERCA_SOLUCION_H
