//
// Created by Fernando Cores Prado on 9/4/24.
//

#ifndef ARBOL_H
#define ARBOL_H

#include <stdbool.h>

#include "Utils.h"


// Definición estructura Coordenada.
struct Point
{
    int 	x;
    int 	y;
};
typedef struct Point Point, *PtrPoint;

// Definición estructura arbol entrada (Conjunto árboles).
struct  Arbol
{
    int	    IdArbol;
    Point   Coord;			// Posicin árbol
    int     Valor;				// Valor / Coste árbol.
    int     Longitud;			// Cantidad madera árbol
};
typedef struct Arbol TArbol, *PtrArbol;

// Definición estructura Bosque entrada (Conjunto árboles).
struct Bosque
{
    int 	NumArboles;
    TArbol 	Arboles[DMaxArboles];
};
typedef struct Bosque TBosque, *PtrBosque;


// Definición estructura lista árboles.
struct ListaArboles {
    int NumArboles;
    int Arboles[DMaxArboles];
};
typedef struct ListaArboles TListaArboles, *PtrListaArboles;

// Vector estático Coordenadas.
typedef Point TVectorCoordenadas[DMaxArboles], *PtrVectorCoordenadas;


// Variables Globales:
extern TBosque ArbolesEntrada;
extern TBosque ArbolesEntradaOrdenados;


// Prototipoes Funciones:
bool LeerFicheroEntrada(char *PathFicIn);
void PrintArbol(int arbol);
void MostrarArbolesLista(TListaArboles listaArboles);
void MostrarInfoArbolesLista(TListaArboles listaArboles);


#endif // ARBOL_H
