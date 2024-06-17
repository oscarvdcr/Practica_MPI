/* ---------------------------------------------------------------
Práctica 1.
Código fuente: CalcArbolesBB_MPI.c
Grau Informática
49254458G van de Crommert Rodoreda, Òscar.
48057785K Gonzàlez Saló, Marc
--------------------------------------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "CalcArbolesEX.h"
#include "ConvexHull.h"
#include "Utils.h"
#include <mpi.h>

// Variables Globales
int rank, size;
struct timespec start, finish;
double elapsed_std;
TCombinacionArboles Combinaciones;
TSolucionArboles Optimo;
TCombinacionArboles MaxCombinaciones;


// Calcular cerca óptima mediante el método exhaustivo
bool CalcularCercaOptimaExhaustiva(PtrSolucionArboles solucion)
{
    clock_gettime(CLOCK_MONOTONIC, &start);

	// Ordenar Arboles por segun coordenadas crecientes de x,y
	OrdenarArboles();

	/* Cálculo óptimo */
	Optimo.ArbolesTalados.NumArboles = 0;
	Optimo.Coste = DMaximoCoste;
    //Versió Paral·lela mpiexec -n 4 ./CalcCercaMPI ConjuntoPruevas/EjemploGrande1_25arboles.dat
	int coste = RepartirTrabajo();
    
    //Versió iterativa ./CalcCercaMPI ConjuntoPruevas/EjemploGrande1_25arboles.dat
    //TCombinacionArboles MaxCombinaciones = (int) pow(2.0,ArbolesEntrada.NumArboles), inicio, final;
    //int coste = CalcularCombinacionOptima(0,MaxCombinaciones);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed_std = (finish.tv_sec - start.tv_sec);
    elapsed_std += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    if (rank == 0){
        memcpy(solucion,&Optimo,sizeof(TSolucionArboles));
        printf("\n[CalcularCercaOptimaExhaustiva] Tiempo requerido cálculo cerca optima: %05.3f segs. %lu combinaciones evaluadas\n",elapsed_std, MaxCombinaciones);
    }
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

int RepartirTrabajo() {
    long trabajos;
    MaxCombinaciones = (int) pow(2.0,ArbolesEntrada.NumArboles);
    TCombinacionArboles inicio, final;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &size );

    trabajos = MaxCombinaciones/size;

    inicio = rank*trabajos;
    //If MaxCombinaciones%size!=0 means that the last combination to evaluate will be excluded
    //By doing this we secure that we evaluate that combination and assign it to the last process
    if (rank==size-1 && MaxCombinaciones%size != 0) {
        final = ((rank+1)*trabajos);
    }else{
        final = ((rank+1)*trabajos)-1;
    }

    MPI_Barrier(MPI_COMM_WORLD);//To print correctly the entry data
    
    //Returns optimal cost of the combinations assigned to them
    //The information is stored in Global Variable Optima    
    CalcularCombinacionOptima(inicio, final); 
     //We recive the information and compare it to the optimal solution stored atm
     //We send the struct of the optimal solution to the process with rank 0
    TSolucionArboles recived;
    if(rank == 0) {
        for(int i=0; i<size-1; i++ ) {
            recived = recv_unpack_optimo();
            if(recived.Coste < Optimo.Coste) {
                Optimo = recived;
            }                      
        }
        return Optimo.Coste;
    } else {
        pack_send_optimo(Optimo);
    }
}
void pack_send_optimo(TSolucionArboles solucion){
    int position = 0, bufsize = 1000;
    char buf[1000];
    
    MPI_Pack(&solucion.Combinacion, 1, MPI_UNSIGNED_LONG, buf, bufsize, &position, MPI_COMM_WORLD);
    MPI_Pack(&solucion.ArbolesTalados.NumArboles, 1, MPI_INT, buf, bufsize, &position, MPI_COMM_WORLD);
    MPI_Pack(solucion.ArbolesTalados.Arboles, solucion.ArbolesTalados.NumArboles, MPI_INT, buf, bufsize, &position, MPI_COMM_WORLD);
    MPI_Pack(&solucion.Coste, 1, MPI_INT, buf, bufsize, &position, MPI_COMM_WORLD);
    MPI_Pack(&solucion.CosteArbolesRestantes, 1, MPI_INT, buf, bufsize, &position, MPI_COMM_WORLD);
    MPI_Pack(&solucion.LongitudCerca, 1, MPI_FLOAT, buf, bufsize, &position, MPI_COMM_WORLD);
    MPI_Pack(&solucion.MaderaSobrante, 1, MPI_FLOAT, buf, bufsize, &position, MPI_COMM_WORLD);

    MPI_Send(buf, position, MPI_PACKED, 0, 0, MPI_COMM_WORLD);

}

TSolucionArboles recv_unpack_optimo(){
    int position = 0, bufsize = 1000;
    char buf[1000];
    MPI_Status status;
    TSolucionArboles received_solucion;

    MPI_Recv(buf, bufsize, MPI_PACKED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

    MPI_Unpack(buf, bufsize, &position, &received_solucion.Combinacion, 1, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufsize, &position, &received_solucion.ArbolesTalados.NumArboles, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufsize, &position, &received_solucion.ArbolesTalados.Arboles, received_solucion.ArbolesTalados.NumArboles, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufsize, &position, &received_solucion.Coste, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufsize, &position, &received_solucion.CosteArbolesRestantes, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufsize, &position, &received_solucion.LongitudCerca, 1, MPI_FLOAT, MPI_COMM_WORLD);
    MPI_Unpack(buf, bufsize, &position, &received_solucion.MaderaSobrante, 1, MPI_FLOAT, MPI_COMM_WORLD);

    return received_solucion;
}

// Calcula la combinación óptima entre el rango de combinaciones PrimeraCombinacion-UltimaCombinacion.
int CalcularCombinacionOptima(TCombinacionArboles PrimeraCombinacion, TCombinacionArboles UltimaCombinacion)
{
    TListaArboles listaArbolesTalados;
    if (rank == 0)
  	    printf("\n[CalcularCercaOptimaExhaustiva] Evaluación Combinaciones posibles: \n");
	for (Combinaciones=PrimeraCombinacion; Combinaciones<UltimaCombinacion; Combinaciones++)
	{
   	    if (DShowAllCombinations) printf("\tC%lu -> \t",(unsigned long) Combinaciones);
        EvaluarCosteCombinacion(Combinaciones);
	}

    ConvertirCombinacionToArbolesTalados(Optimo.Combinacion, &listaArbolesTalados);
    MPI_Barrier(MPI_COMM_WORLD);
    return Optimo.Coste;
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






	
