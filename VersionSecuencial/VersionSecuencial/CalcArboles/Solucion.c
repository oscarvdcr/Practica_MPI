//
// Created by Fernando Cores Prado on 10/4/24.
//

#include "Solucion.h"

#include <stdio.h>


bool GenerarFicheroSalida(TSolucionArboles Optimo, char *PathFicOut)
{
    FILE *FicOut;
    int a;

    FicOut=fopen(PathFicOut,"w+");
    if (FicOut==NULL)
    {
        perror("Escritura fichero salida.");
        return false;
    }

    // Escribir arboles a talartalado.
    // Escribimos nmero de arboles a talar.
    if (fprintf(FicOut, "Cortar %d árbol/es: ", Optimo.ArbolesTalados.NumArboles)<1)
    {
        perror("Escribir nmero de arboles a talar");
        return false;
    }

    for(a=0;a<Optimo.ArbolesTalados.NumArboles;a++)
    {
        // Escribir número arbol.
        if (fprintf(FicOut, "%d ",ArbolesEntradaOrdenados.Arboles[Optimo.ArbolesTalados.Arboles[a]].IdArbol)<1)
        {
            perror("Escritura nmero árbol.");
            return false;
        }
    }

    // Escribimos madera sobrante.
    if (fprintf(FicOut, "\nMadera Sobrante: \t%4.2f (%4.2f)", Optimo.MaderaSobrante,  Optimo.LongitudCerca)<1)
    {
        perror("Escribir coste arboles a talar.");
        return false;
    }

    // Escribimos coste arboles a talar.
    if (fprintf(FicOut, "\nValor árboles cortados: \t%d.", Optimo.Coste)<1)
    {
        perror("Escribir coste arboles a talar.");
        return false;
    }

    // Escribimos coste arboles a talar.
    if (fprintf(FicOut, "\nValor árboles restantes: \t%d\n", Optimo.CosteArbolesRestantes)<1)
    {
        perror("Escribir coste arboles a talar.");
        return false;
    }

    return true;
}


void MostrarArboles(TSolucionArboles CombinacionArboles)
{
    int a;

    for (a=0;a<CombinacionArboles.ArbolesTalados.NumArboles;a++)
        printf("%d ",ArbolesEntradaOrdenados.Arboles[CombinacionArboles.ArbolesTalados.Arboles[a]].IdArbol);

    for (;a<ArbolesEntradaOrdenados.NumArboles;a++)
        printf("  ");
}

void MostrarInfoArboles(TSolucionArboles CombinacionArboles)
{
    int a;

    for (a=0;a<CombinacionArboles.ArbolesTalados.NumArboles;a++)
        PrintArbol(CombinacionArboles.ArbolesTalados.Arboles[a]);
}




