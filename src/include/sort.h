#include "src/include/rtree.h"
#ifndef SORT_H
#define SORT_H

/*
 * Intercambia dos elementos en los arreglos paralelos de CenterRectagle y Pair.
 * Se usan arreglos paralelos para mantener la correspondencia entre centros y pares
 * durante el ordenamiento.
 * @param a,b  Elementos de CenterRectagle a intercambiar.
 * @param c,d  Elementos de Pair correspondientes a intercambiar.
 */
void swap(CenterRectagle *a, CenterRectagle *b, Pair *c, Pair *d);

/*
 * Función de partición para quicksort sobre arreglos paralelos CenterRectagle/Pair.
 * Usa el elemento en la posición high-1 como pivote.
 * @param a     Arreglo de CenterRectagle a particionar.
 * @param low   Índice inferior del subarreglo.
 * @param high  Índice superior (exclusivo) del subarreglo.
 * @param c     Arreglo de Pair paralelo a a.
 * @return      Índice del pivote tras la partición.
 */
int particionar(CenterRectagle a[], int low, int high, Pair c[]);

/*
 * Ordena los arreglos paralelos a[] y c[] por el campo middleX usando quicksort.
 * @param a  Arreglo de CenterRectagle a ordenar.
 * @param i  Índice inferior del subarreglo.
 * @param j  Índice superior del subarreglo.
 * @param c  Arreglo de Pair paralelo a a.
 */
void quicksort_seq(CenterRectagle a[], int i, int j, Pair c[]);

#endif // SORT_H