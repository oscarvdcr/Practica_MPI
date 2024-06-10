
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "CalcArbolesEX.h"
#include "ConvexHull.h"
#include "Utils.h"
#include <linux/time.h>
#include <mpi.h>

// Variables Globales
struct timespec start, finish;
double elapsed_std;
TCombinacionArboles Combinaciones;
TSolucionArboles Optimo;


// Calcular cerca óptima mediante el método exhaustivo
bool CalcularCercaOptimaExhaustiva(PtrSolucionArboles solucion)
{
	TCombinacionArboles MaxCombinaciones;

    clock_gettime(CLOCK_MONOTONIC, &start);

	/* Cálculo Máximo Combinaciones */
	MaxCombinaciones = (int) pow(2.0,ArbolesEntrada.NumArboles);



	// Ordenar Arboles por segun coordenadas crecientes de x,y
	OrdenarArboles();

	/* Cálculo óptimo */
	Optimo.ArbolesTalados.NumArboles = 0;
	Optimo.Coste = DMaximoCoste;
	int coste = CalcularCombinacionOptima(1, MaxCombinaciones);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed_std = (finish.tv_sec - start.tv_sec);
    elapsed_std += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    memcpy(solucion,&Optimo,sizeof(TSolucionArboles));

    printf("[CalcularCercaOptimaExhaustiva] Tiempo requerido cálculo cerca optima: %05.3f segs. %lu combinaciones evaluadas\n",elapsed_std, (unsigned long) Combinaciones);

    if (coste==DMaximoCoste)
        return false;
    else
	    return true;
}

// Método para ordenar los árboles de menor a mayor por la coordendas (primero por x y despues por y)
void OrdenarArboles()
{
    int a,b;

    memcpy(&ArbolesEntradaOrdenados,&ArbolesEntrada, sizeof(ArbolesEntrada));
	for(a=0; a<(ArbolesEntradaOrdenados.NumArboles-1); a++)
	{
		for(b=1; b<ArbolesEntradaOrdenados.NumArboles; b++)
		{
			if ( ArbolesEntradaOrdenados.Arboles[b].Coord.x < ArbolesEntradaOrdenados.Arboles[a].Coord.x ||
				 (ArbolesEntradaOrdenados.Arboles[b].Coord.x == ArbolesEntradaOrdenados.Arboles[a].Coord.x && ArbolesEntradaOrdenados.Arboles[b].Coord.y < ArbolesEntradaOrdenados.Arboles[a].Coord.y) )
			{
				TArbol aux;

				// aux=a
				aux.Coord.x = ArbolesEntradaOrdenados.Arboles[a].Coord.x;
				aux.Coord.y = ArbolesEntradaOrdenados.Arboles[a].Coord.y;
				aux.IdArbol = ArbolesEntradaOrdenados.Arboles[a].IdArbol;
				aux.Valor = ArbolesEntradaOrdenados.Arboles[a].Valor;
				aux.Longitud = ArbolesEntradaOrdenados.Arboles[a].Longitud;

				// a=b
                ArbolesEntradaOrdenados.Arboles[a].Coord.x = ArbolesEntradaOrdenados.Arboles[b].Coord.x;
                ArbolesEntradaOrdenados.Arboles[a].Coord.y = ArbolesEntradaOrdenados.Arboles[b].Coord.y;
                ArbolesEntradaOrdenados.Arboles[a].IdArbol = ArbolesEntradaOrdenados.Arboles[b].IdArbol;
				ArbolesEntradaOrdenados.Arboles[a].Valor = ArbolesEntradaOrdenados.Arboles[b].Valor;
				ArbolesEntradaOrdenados.Arboles[a].Longitud = ArbolesEntradaOrdenados.Arboles[b].Longitud;

				// b=aux
				ArbolesEntradaOrdenados.Arboles[b].Coord.x = aux.Coord.x;
				ArbolesEntradaOrdenados.Arboles[b].Coord.y = aux.Coord.y;
				ArbolesEntradaOrdenados.Arboles[b].IdArbol = aux.IdArbol;
				ArbolesEntradaOrdenados.Arboles[b].Valor = aux.Valor;
				ArbolesEntradaOrdenados.Arboles[b].Longitud = aux.Longitud;
			}
		}
	}
}

struct {
    TSolucionArboles Optimo;
    int coste; 
} Resultados;

int RepartirTrabajo() {
    TCombinacionArboles MaxCombinaciones = (int) pow(2.0,ArbolesEntrada.NumArboles);
    int rank, size, posicion, costeOptimo = 9999999, coste;
    TCombinacionArboles mejorCombinacion, patata;
    char buff[1000];
    TSolucionArboles Optimo;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM comm, int &size);
    MPI_Comm_rank(MPI_Comm comm, int &rank); 
    long trabajos = MaxCombinaciones/size;

    TCombinacionArboles inicio = rank*MaxCombinaciones;
    TCombinacionArboles final = (rank+1)*MaxCombinaciones-1;

    Optimo = CalcularCombinacionOptima(inicio, final);
    
    if(rank == 0) {
        
        MPI_Recv(buff, 1000 , MPI_PACKED, 0, 0, &status);//TODO: do it with structs
        posicion = 0;
        for(int i = 0; i < size; i++) {
            MPI_Unpack(buff, 1000, &posicion, &coste, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Unpack(buff, 1000, &posicion, &patata, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);

            if (coste < costeOptimo)
            {
                costeOptimo = coste;
                mejorCombinacion = patata;
            }
            
        }

    } else {
        posicion = 0;
        MPI_Pack(&Optimo.Coste);
        MPI_Pack(&Optimo.Combinacion);

        MPI_Send(buff, posicion, MPI_PACKED, 1, 0, MPI_COMM_WORLD); //TODO: do it with structs
    }

    MPI_Finalize();
    return costeOptimo;
}


// Calcula la combinación óptima entre el rango de combinaciones PrimeraCombinacion-UltimaCombinacion.
TSolucionArboles CalcularCombinacionOptima(TCombinacionArboles PrimeraCombinacion, TCombinacionArboles UltimaCombinacion)
{
    TListaArboles listaArbolesTalados;

  	printf("\n[CalcularCercaOptimaExhaustiva] Evaluación Combinaciones posibles: \n");
	for (Combinaciones=PrimeraCombinacion; Combinaciones<UltimaCombinacion; Combinaciones++)
	{
   	    if (DShowAllCombinations) printf("\tC%lu -> \t",(unsigned long) Combinaciones);
        EvaluarCosteCombinacion(Combinaciones);
	}

    ConvertirCombinacionToArbolesTalados(Optimo.Combinacion, &listaArbolesTalados);
    printf("\r\tOptimo %lu-> Coste %d, %d Arboles talados:", (long int) Optimo.Combinacion, Optimo.Coste, Optimo.ArbolesTalados.NumArboles);
    MostrarArboles(Optimo);
    MostrarInfoArbolesLista(Optimo.ArbolesTalados);
    printf("\n");

	return Optimo;
}


// Evaluamos el coste de una combinación.
int EvaluarCosteCombinacion(TCombinacionArboles Combinacion)
{
    TSolucionArboles solucion;

    solucion.Combinacion = Combinacion;
    CalcularCosteCerca(&solucion);

    return(solucion.Coste);
}

// Calcular coste posible solución.
int CalcularCosteCerca(PtrSolucionArboles solucion) {
    TListaArboles listaArbolesNoCortados, listaArbolesTalados;
    TVectorCoordenadas CoordArboles, CercaArboles;
    int puntosCerca;
    int valor_arboles, coste_arboles_talados, maderaArbolesTalados;
    float longitudCerca;
    bool OptimoEncontrado = false;

    // Evaluar el coste y la madera obtenida mediante los arboles talados.
    // Convertimos la combinaciónn al vector de arboles no talados.
    coste_arboles_talados = ConvertirCombinacionToArbolesTalados(solucion->Combinacion, &(solucion->ArbolesTalados));
    maderaArbolesTalados = CalcularMaderaArbolesTalados(solucion->ArbolesTalados);

    // Calcular longitud cerca
    valor_arboles = ConvertirCombinacionToArboles(solucion->Combinacion, &listaArbolesNoCortados);
    ObtenerListaCoordenadasArboles(listaArbolesNoCortados, CoordArboles);
    puntosCerca = chainHull_2D(CoordArboles, listaArbolesNoCortados.NumArboles, CercaArboles);
    longitudCerca = CalcularLongitudCerca(CercaArboles, puntosCerca);

    // ASignamos valores solución.
    solucion->CosteArbolesRestantes = valor_arboles;
    solucion->LongitudCerca = longitudCerca;
    solucion->MaderaSobrante = maderaArbolesTalados - solucion->LongitudCerca;

    if (DShowAllCombinations) printf("\tC%lu -> \t", (long int) solucion->Combinacion);
    if (DShowAllCombinations) printf(" %d arboles cortados: ", listaArbolesTalados.NumArboles);
    if (DShowAllCombinations) MostrarArbolesLista(listaArbolesTalados);
    if (DShowAllCombinations) printf("  Madera:%d  \tCerca:%4.2f ", maderaArbolesTalados, longitudCerca);

    if (longitudCerca >
        maderaArbolesTalados) {   // Los arboles cortados no tienen suficiente madera para construir la cerca.
        solucion->Coste = DMaximoCoste;
        if (DShowAllCombinations) printf(" \tCoste: MAX.\n");
    } else {   // Tenemos suficiente madera para realizar la cerca.
        solucion->Coste = coste_arboles_talados;
        if (DShowAllCombinations) printf(" \tCoste:%d.\n", solucion->Coste);
    }

    if (solucion->Coste != DMaximoCoste && solucion->Coste < Optimo.Coste) {
        memcpy(&Optimo, solucion, sizeof(TSolucionArboles));
        OptimoEncontrado = true;
        if (DShowAllCombinations) printf(" *****");
    }

    if ((Combinaciones%S)==0 || OptimoEncontrado)
    {
        ConvertirCombinacionToArbolesTalados(solucion->Combinacion, &listaArbolesTalados);
        memcpy(solucion->ArbolesTalados.Arboles, listaArbolesTalados.Arboles, sizeof(listaArbolesTalados.Arboles));
        if (!DShowAllCombinations) printf("\r[%10lu] OptimoParcial %10lu-> Coste %d,  %d Arbol/es talado/s: ", (long int) Combinaciones, (long int)Optimo.Combinacion, Optimo.Coste, Optimo.ArbolesTalados.NumArboles);
        if (!DShowAllCombinations) MostrarArboles(Optimo);
        if (OptimoEncontrado)
            if (!DShowAllCombinations) printf("\n");
    }

    return solucion->Coste;
}


// Metodo que convierte una posible solución (combinación binaria de árboles) a la lista de árboles
// que no se van a talar.
int ConvertirCombinacionToArboles(TCombinacionArboles combinacion, PtrListaArboles listaArboles)
{
    int arbol=0, coste;

    listaArboles->NumArboles=0;
    coste=0;
    while (arbol<ArbolesEntradaOrdenados.NumArboles)
    {
        if ((combinacion%2)==0)
        {
            listaArboles->Arboles[listaArboles->NumArboles]=arbol;
            listaArboles->NumArboles++;
            coste+= ArbolesEntradaOrdenados.Arboles[arbol].Valor;
        }
        arbol++;
        combinacion = combinacion>>1;
    }

    return coste;
}

// Metodo que convierte una posible solución (combinación binaria de árboles) a la lista de árboles
// que a talar.
int ConvertirCombinacionToArbolesTalados(TCombinacionArboles combinacion, PtrListaArboles listaArbolesTalados)
{
    int arbol=0, coste=0;

    listaArbolesTalados->NumArboles=0;
    while (arbol<ArbolesEntradaOrdenados.NumArboles)
    {
        if ((combinacion % 2) == 1)
        {
            listaArbolesTalados->Arboles[listaArbolesTalados->NumArboles]=arbol;
            listaArbolesTalados->NumArboles++;
            coste+= ArbolesEntradaOrdenados.Arboles[arbol].Valor;
        }
        arbol++;
        combinacion = combinacion >> 1;
    }

    return coste;
}

// A partir de una lista de árboles obtiene su vector de coordenadas.
void ObtenerListaCoordenadasArboles(TListaArboles listaArboles, TVectorCoordenadas Coordenadas)
{
    int c, arbol;

    for (c=0;c<listaArboles.NumArboles;c++)
    {
        arbol=listaArboles.Arboles[c];
        Coordenadas[c].x = ArbolesEntradaOrdenados.Arboles[arbol].Coord.x;
        Coordenadas[c].y = ArbolesEntradaOrdenados.Arboles[arbol].Coord.y;
    }
}

// Método para calcula la longitud de la vaya que rodea a conjunto de coordenasdas (vector)
float CalcularLongitudCerca(TVectorCoordenadas CoordenadasCerca, int SizeCerca)
{
	int x;
	float coste=0;
	
	for (x=0;x<(SizeCerca-1);x++)
	{
        // Calcular distancia entro dos coordenadas
		coste+= CalcularDistancia(CoordenadasCerca[x].x, CoordenadasCerca[x].y, CoordenadasCerca[x+1].x, CoordenadasCerca[x+1].y);
	}
	
	return coste;
}


float CalcularDistancia(int x1, int y1, int x2, int y2)
{
	return(sqrt(pow((double)abs(x2-x1),2.0)+pow((double)abs(y2-y1),2.0)));
}

// A partir de una lista de árboles talados cálcula la madera talada disponible para la cerca.
int CalcularMaderaArbolesTalados(TListaArboles listaArboles)
{
    int a;
    int LongitudTotal=0;

    for (a=0;a<listaArboles.NumArboles;a++)
    {
        LongitudTotal += ArbolesEntradaOrdenados.Arboles[listaArboles.Arboles[a]].Longitud;
    }

    return(LongitudTotal);
}


// Método para calcular el conste de los árboles talaados de una solución
int 
CalcularCosteCombinacion(TSolucionArboles CombinacionArboles)
{
	int a;
	int CosteTotal=0;
	
	for (a=0;a<CombinacionArboles.ArbolesTalados.NumArboles;a++)
	{
		CosteTotal += ArbolesEntradaOrdenados.Arboles[CombinacionArboles.ArbolesTalados.Arboles[a]].Valor;
	}
	
	return(CosteTotal);
}






	
