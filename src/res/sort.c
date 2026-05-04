// #include "include/sort.h"
// #include "include/rtree.h"
#include "src/include/sort.h"


void swap(CenterRectagle *a, CenterRectagle *b, Pair *c, Pair *d) {
    CenterRectagle t = *a;
    *a = *b;
    *b = t;
    Pair u = *c;
    *c = *d;
    *d = u;
}
int particionar(CenterRectagle a[], int low, int high, Pair c[]) {
    float pivot = a[high - 1].middleX;  
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (a[j].middleX <= pivot) {
            i++;
            swap(&a[i], &a[j], &c[i], &c[j]);
        }
    }
    return i + 1;
}
void quicksort_seq(CenterRectagle a[], int i, int j, Pair c[]){
    if (i < j){
        int h = particionar(a, i, j, c);
        quicksort_seq(a, i, h - 1, c);
        quicksort_seq(a, h + 1, j, c);
    }
}