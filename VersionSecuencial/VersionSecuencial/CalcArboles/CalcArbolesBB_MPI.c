/* ---------------------------------------------------------------
Práctica 1.
Código fuente: CalcArbolesBB_MPI.c
Grau Informática
49254458G van de Crommert Rodoreda, Òscar.
48057785K Gonzàlez Saló, Marc
--------------------------------------------------------------- */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "CalcArbolesBB.h"
#include "CalcArbolesEX.h"
#include "ConvexHull.h"
#include "Utils.h"

// Metodo que calcula la cerca óptima mediante el método de Branch & Bound.
int CalcularCercaOptimaBrandBound(PtrSolucionArboles solucion)
{
    TListaArboles listaArbolesTalados;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Ordenar Arboles por segun coordenadas crecientes de x,y
    OrdenarArboles();

    /* Inicializar óptimo */
    Optimo.ArbolesTalados.NumArboles = 0;
    Optimo.Coste = DMaximoCoste;
    Optimo.Combinacion = 0;

    /* Inicializar primera combinación */
    solucion->ArbolesTalados.NumArboles = 0;
    solucion->Coste = DMaximoCoste;
    solucion->Combinacion = 0; // Sin árboles a talar
    Combinaciones=0;

    printf("\n[CalcularCercaOptimaBrandBound] Evaluación Combinaciones posibles: \n");
    CalcularCercaOptimaBB(0, solucion);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed_std = (finish.tv_sec - start.tv_sec);
    elapsed_std += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    ConvertirCombinacionToArbolesTalados(solucion->Combinacion, &listaArbolesTalados);
    printf("\r\tOptimo %lu-> Coste %d, %d Arboles talados:", (long int) solucion->Combinacion, solucion->Coste, solucion->ArbolesTalados.NumArboles);
    MostrarArbolesLista(listaArbolesTalados);
    MostrarInfoArbolesLista(listaArbolesTalados);
    printf("\n");

    printf("[CalcularCercaOptimaBrandBound] Tiempo requerido cálculo cerca optima: %05.3f segs. %lu combinaciones evaluadas\n",elapsed_std, (long int)Combinaciones);

    return solucion->Coste;
}


// Metodo recursivo para calcular la cerca óptima mediante el método de Branch & Bound.
int CalcularCercaOptimaBB(int a, PtrSolucionArboles solucion)
{
    // No quedan más árboles que cortar.
    if (a==ArbolesEntradaOrdenados.NumArboles)
        return solucion->Coste;

    // Hemos encontrado una solución
    if (solucion->Coste!=DMaximoCoste && solucion->MaderaSobrante>=0)
        return solucion->Coste;

    // Poda de esta rama del arbol si el coste actual es mayor que el coste del óptimo:
    if (solucion->Coste!=DMaximoCoste && solucion->Coste>Optimo.Coste)
        return solucion->Coste;

    // Solución cortando arbol a
    PtrSolucionArboles solucion_con_arbol = AnyadirArbolSolucion(solucion, a);
    int coste_con_arbol = CalcularCercaOptimaBB(a+1, solucion_con_arbol);

    // Solución sin cortar arbol a
    int coste_sin_arbol = CalcularCercaOptimaBB(a+1, solucion);

    if (coste_con_arbol<coste_sin_arbol){
        //free(solucion);
        //solucion = solucion_con_arbol;
        memcpy(solucion,solucion_con_arbol,sizeof(TSolucionArboles));
        return coste_con_arbol;
    }
    else{
        free(solucion_con_arbol);
        return coste_sin_arbol;
    }
}

// Método para incluir un árbol talado en una solución
PtrSolucionArboles AnyadirArbolSolucion(PtrSolucionArboles solucion, int arbol)
{
    PtrSolucionArboles new_sol = malloc(sizeof(TSolucionArboles));
    if (new_sol==NULL)
        Error("Error reservando memoria para nueva solucion.");

    memcpy(new_sol,solucion,sizeof(TSolucionArboles));

    // Anyadimos el árbol a la combinación de la solución.
    CortarArbolCombinacion(&(new_sol->Combinacion), arbol);
    new_sol->ArbolesTalados.NumArboles++;

    // Calcular costes nueva solución
    CalcularCosteCerca(new_sol);

    Combinaciones++;

    return new_sol;
}


void CortarArbolCombinacion(PtrCombinacionArboles combinacion, int arbol)
{
    // Activamos bit correspondiente al árbol
    (*combinacion) |= (int)pow(2, arbol);
}