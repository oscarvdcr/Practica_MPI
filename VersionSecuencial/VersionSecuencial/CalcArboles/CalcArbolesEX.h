//
// Created by Fernando Cores Prado on 10/4/24.
//

#ifndef CALCCERCA_CALCARBOLESEX_H
#define CALCCERCA_CALCARBOLESEX_H

#include <stdbool.h>

#include "Solucion.h"
#include "Arbol.h"
#include "CalcCerca.h"

// Constantes:
#define S 100000

// Variables Globales
extern struct timespec start, finish;
extern double elapsed_std;
extern TCombinacionArboles Combinaciones;
extern TSolucionArboles Optimo;


// Prototipos funciones:
bool CalcularCercaOptimaExhaustiva(PtrSolucionArboles Optimo);
void OrdenarArboles();
int CalcularCombinacionOptima(TCombinacionArboles PrimeraCombinacion, TCombinacionArboles UltimaCombinacion);
int EvaluarCosteCombinacion(TCombinacionArboles Combinacion);
int CalcularCosteCerca(PtrSolucionArboles solucion);
int ConvertirCombinacionToArboles(TCombinacionArboles combinacion, PtrListaArboles listaArboles);
int ConvertirCombinacionToArbolesTalados(TCombinacionArboles combinacion, PtrListaArboles listaArbolesTalados);
void ObtenerListaCoordenadasArboles(TListaArboles listaArboles, TVectorCoordenadas Coordenadas);
float CalcularLongitudCerca(TVectorCoordenadas CoordenadasCerca, int SizeCerca);
float CalcularDistancia(int x1, int y1, int x2, int y2);
int CalcularMaderaArbolesTalados(TListaArboles listaArboles);
void MostrarArboles(TSolucionArboles CombinacionArboles);

int RepartirTrabajo();
void pack_send_optimo(TSolucionArboles optimo);
TSolucionArboles recv_unpack_optimo();

#endif //CALCCERCA_CALCARBOLESEX_H
