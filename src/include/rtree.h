#include <stdio.h>
#include <stdlib.h>

#ifndef RTREE_H
#define RTREE_h

    #define B 204 // el valor asignado a b en el enunciado
    #define FALSE 0
    #define TRUE 1
    typedef enum{
        RAIZ,
        NUEVO_NODO
    }Insercion;

    typedef struct rectangle
    {
        float x1,x2,y1,y2;
    } Rectangle;

    typedef struct pair             
    {
        Rectangle key;
        int value;
    } Pair;


    typedef struct node
    {
        int k; // cantidad de hijos actualmente contenidos en el nodo. k puede fluctuar entre 1 y B
        Pair hijos[B]; //arreglo de hijos
        char pad[12]; 

    }Node;

    typedef struct rtree{
        int nodes;
        FILE *tree;
    }RTree;

    typedef struct center
    {
        float middleX;
        Pair *rectangle;
    }CenterRectagle;

    Rectangle* createRectangle(float x1, float x2, float y1, float y2);
    Node *createNode(Pair *children, int n, FILE *f);
    RTree* createRTree(char *name);
    int savePair(Pair *tuple, FILE *f);
    int parsePair(Pair *tuple, FILE *f);
    int saveNode(Node *nodo, FILE *f);
    int parseNode(Node *nodo, FILE *f, int pos);
    void addChildrenToNode(Node *node, Pair *children, int n);
    //void freeNode(Node *node);
    void freeRTree(RTree *tree);
    int addNodeTotree(RTree *tree,Node *node, Insercion insercion);
    void interseccion(Rectangle *rect, Node *nodo, FILE *puntos, int *cantidadPuntos, RTree *tree);

#endif