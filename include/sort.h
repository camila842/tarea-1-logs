#include "include/rtree.h"
#ifndef SORT_H
#define SORT_H

// Intercambia dos elementos
void swap(CenterRectagle *a, CenterRectagle *b, Pair *c, Pair *d);

// Función de partición para quicksort
int particionar(CenterRectagle a[], int low, int high, Pair c[]);

// Quicksort secuencial
void quicksort_seq(CenterRectagle a[], int i, int j, Pair c[]);

#endif // SORT_H