/* ---------------------------------------------------------------
Práctica 1.
Código fuente: Arbol.c
Grau Informática
49254458G van de Crommert Rodoreda, Òscar.
48057785K Gonzàlez Saló, Marc
--------------------------------------------------------------- */

#include "Arbol.h"

#include <stdio.h>
#include <mpi.h>

// Variables Globales:
TBosque ArbolesEntrada;
TBosque ArbolesEntradaOrdenados;



bool LeerFicheroEntrada(char *PathFicIn)
{
    FILE *FicIn;
    int a, rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    FicIn=fopen(PathFicIn,"r");
    if (FicIn==NULL)
    {
        perror("Lectura Fichero entrada.");
        return false;
    }
    if(rank == 0)
        printf("Datos Entrada:\n");

    // Leemos el nmero de arboles del bosque de entrada.
    if (fscanf(FicIn, "%d", &(ArbolesEntrada.NumArboles))<1)
    {
        perror("Lectura arboles entrada");
        return false;
    }
    if (rank == 0)
        printf("\tArboles: %d.\n",ArbolesEntrada.NumArboles);

    // Leer atributos arboles.
    for(a=0;a<ArbolesEntrada.NumArboles;a++)
    {
        ArbolesEntrada.Arboles[a].IdArbol=a+1;
        // Leer x, y, Coste, Longitud.
        if (fscanf(FicIn, "%d %d %d %d",&(ArbolesEntrada.Arboles[a].Coord.x), &(ArbolesEntrada.Arboles[a].Coord.y), &(ArbolesEntrada.Arboles[a].Valor), &(ArbolesEntrada.Arboles[a].Longitud))<4)
        {
            perror("Lectura datos arbol.");
            return false;
        }
        if (rank == 0)
            printf("\tArbol %d-> (%d,%d) Coste:%d, Long:%d.\n",a+1,ArbolesEntrada.Arboles[a].Coord.x, ArbolesEntrada.Arboles[a].Coord.y, ArbolesEntrada.Arboles[a].Valor, ArbolesEntrada.Arboles[a].Longitud);
    }

    return true;
}


void PrintArbol(int arbol)
{
    printf("\n\t\t\t");
    printf(" \tId: %2d",ArbolesEntradaOrdenados.Arboles[arbol].IdArbol);
    printf(" \tCoord: (%d,%d)",ArbolesEntradaOrdenados.Arboles[arbol].Coord.x, ArbolesEntradaOrdenados.Arboles[arbol].Coord.y);
    printf(" \tCost: %3d",ArbolesEntradaOrdenados.Arboles[arbol].Valor);
    printf(" \tLong: %3d",ArbolesEntradaOrdenados.Arboles[arbol].Longitud);
}

void MostrarArbolesLista(TListaArboles listaArboles)
{
    int a;

    for (a=0;a<listaArboles.NumArboles;a++)
        printf("%d ",ArbolesEntradaOrdenados.Arboles[listaArboles.Arboles[a]].IdArbol);
}

void MostrarInfoArbolesLista(TListaArboles listaArboles)
{
    int a;

    for (a=0;a<listaArboles.NumArboles;a++)
        PrintArbol(listaArboles.Arboles[a]);
}