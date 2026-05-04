#include "src/include/sort.h"
#include "math.h"

#ifndef BURK_L_H
#define BURK_L_H

    typedef enum{
        NEARESTX,
        STRECURSIVE
    } Function;

    void nearestX(Pair *parLlaveValor, int n, RTree *rtree);
    void sortTileRecursive(Pair *parLlaveValor, int n, RTree *rtree);

#endif