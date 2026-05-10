#include "src/include/sort.h"
#include "math.h"

#ifndef BURK_L_H
#define BURK_L_H

    /* Selector del algoritmo de bulk-loading a utilizar. */
    typedef enum{
        NEARESTX,    /* Algoritmo NearestX: ordena por coordenada X y agrupa secuencialmente */
        STRECURSIVE  /* Algoritmo STR: divide en franjas X y luego Y de forma recursiva */
    } Function;

    /*
     * Construye un R-Tree mediante el algoritmo NearestX bulk-loading.
     * Ordena los n pares por la coordenada X del centroide, los agrupa en hojas de
     * tamaño B y construye los niveles superiores de forma recursiva hasta la raíz.
     * @param parLlaveValor  Arreglo de n Pairs (puntos o MBRs) a insertar.
     * @param n              Cantidad de elementos en parLlaveValor.
     * @param rtree          R-Tree destino donde se escribirán los nodos.
     */
    void nearestX(Pair *parLlaveValor, int n, RTree *rtree);

    /*
     * Construye un R-Tree mediante el algoritmo STR (Sort-Tile-Recursive) bulk-loading.
     * Divide los datos en S franjas ordenadas por X, luego ordena cada franja por Y
     * y las subdivide en grupos de tamaño B para formar las hojas. Recursivo hacia arriba.
     * @param parLlaveValor  Arreglo de n Pairs (puntos o MBRs) a insertar.
     * @param n              Cantidad de elementos en parLlaveValor.
     * @param rtree          R-Tree destino donde se escribirán los nodos.
     */
    void sortTileRecursive(Pair *parLlaveValor, int n, RTree *rtree);

#endif