#include <stdio.h>
#include <stdlib.h>

#ifndef RTREE_H
#define RTREE_h

    #define B 204 // el valor asignado a b en el enunciado
    #define FALSE 0
    #define TRUE 1

    /* Indica si un nodo se escribe como raíz (posición 0 del archivo) o como nuevo nodo al final. */
    typedef enum{
        RAIZ,
        NUEVO_NODO
    }Insercion;

    /* Rectángulo de mínimo bounding box (MBR) en 2D, con esquinas (x1,y1) y (x2,y2). */
    typedef struct rectangle
    {
        float x1,x2,y1,y2;
    } Rectangle;

    /* Par clave-valor donde la clave es un MBR y el valor es la posición del nodo hijo en disco
     * (o -1 si el hijo es un punto hoja). */
    typedef struct pair
    {
        Rectangle key;
        int value;
    } Pair;

    /* Nodo del R-Tree. Almacena hasta B hijos (pares clave-valor).
     * k indica cuántos hijos válidos contiene actualmente. */
    typedef struct node
    {
        int k;        /* cantidad de hijos actualmente contenidos; 1 ≤ k ≤ B */
        Pair hijos[B];
        char pad[12]; /* relleno para alinear el tamaño del nodo en disco */
    }Node;

    /* R-Tree respaldado en un archivo binario.
     * nodes cuenta el total de nodos escritos; tree es el handle del archivo. */
    typedef struct rtree{
        int nodes;
        FILE *tree;
    }RTree;

    /* Estructura auxiliar que asocia un Pair con la coordenada X de su centroide,
     * usada para ordenar rectángulos durante el bulk-loading. */
    typedef struct center
    {
        float middleX;
        Pair *rectangle;
    }CenterRectagle;

    /*
     * Crea un Rectangle en el heap con las coordenadas dadas.
     * @param x1,x2  Extremos horizontales del rectángulo.
     * @param y1,y2  Extremos verticales del rectángulo.
     * @return       Puntero al Rectangle creado, o NULL si falla malloc.
     */
    Rectangle* createRectangle(float x1, float x2, float y1, float y2);

    /*
     * Crea un Node en el heap a partir de un arreglo de Pairs.
     * @param children  Arreglo de pares a copiar como hijos del nodo (puede ser NULL).
     * @param n         Cantidad de hijos a copiar.
     * @param f         Handle del archivo del árbol (no se usa para escribir aquí).
     * @return          Puntero al Node creado, o NULL si falla malloc.
     */
    Node *createNode(Pair *children, int n, FILE *f);

    /*
     * Crea un RTree vacío respaldado por el archivo indicado.
     * Si el archivo no existe lo crea; si existe lo trunca.
     * @param name  Ruta del archivo donde se serializará el árbol.
     * @return      Puntero al RTree creado, o NULL si falla.
     */
    RTree* createRTree(char *name);

    /*
     * Serializa un Pair en el archivo en la posición actual del cursor.
     * @param tuple  Pair a escribir.
     * @param f      Handle del archivo destino.
     * @return       TRUE si la escritura fue exitosa, FALSE en caso contrario.
     */
    int savePair(Pair *tuple, FILE *f);

    /*
     * Deserializa un Pair desde la posición actual del cursor del archivo.
     * @param tuple  Pair donde se almacenará la lectura.
     * @param f      Handle del archivo fuente.
     * @return       TRUE si la lectura fue exitosa, FALSE en caso contrario.
     */
    int parsePair(Pair *tuple, FILE *f);

    /*
     * Serializa un Node completo (cabecera k + B hijos + padding) en el archivo.
     * @param nodo  Nodo a escribir.
     * @param f     Handle del archivo destino.
     * @return      TRUE si la escritura fue exitosa, FALSE en caso contrario.
     */
    int saveNode(Node *nodo, FILE *f);

    /*
     * Deserializa el nodo ubicado en la posición lógica pos dentro del archivo.
     * @param nodo  Nodo donde se almacenará la lectura.
     * @param f     Handle del archivo fuente.
     * @param pos   Índice lógico del nodo (0 = raíz).
     * @return      TRUE si la lectura fue exitosa, FALSE en caso contrario.
     */
    int parseNode(Node *nodo, FILE *f, int pos);

    /*
     * Copia hasta n hijos del arreglo children al nodo dado.
     * No escribe en disco; solo modifica la estructura en memoria.
     * @param node      Nodo destino.
     * @param children  Arreglo de Pairs a copiar.
     * @param n         Cantidad de hijos a copiar (debe ser ≤ B).
     */
    void addChildrenToNode(Node *node, Pair *children, int n);

    /*
     * Libera la memoria del RTree y cierra su archivo asociado.
     * @param tree  RTree a liberar.
     */
    void freeRTree(RTree *tree);

    /*
     * Escribe un nodo en el árbol. Si insercion == RAIZ sobreescribe la posición 0;
     * si es NUEVO_NODO lo agrega al final del archivo.
     * @param tree      RTree donde se insertará el nodo.
     * @param node      Nodo a escribir.
     * @param insercion Tipo de inserción (RAIZ o NUEVO_NODO).
     * @return          Número de nodos total tras la inserción, o -1 si falla.
     */
    int addNodeTotree(RTree *tree,Node *node, Insercion insercion);

    /*
     * Busca recursivamente todos los puntos hoja del árbol que intersectan con rect,
     * los escribe en el archivo puntos y contabiliza los accesos a disco.
     * @param rect              Rectángulo de consulta.
     * @param nodo              Nodo actual que se está explorando.
     * @param puntos            Archivo donde se escriben los puntos encontrados.
     * @param tree              R-Tree completo (para leer nodos hijos por posición).
     * @param accesosBusqueda   Contador de nodos internos visitados (lectura).
     * @param accesosInsercion  Contador de puntos hoja escritos.
     */
    void interseccion(Rectangle *rect, Node *nodo, FILE *puntos, RTree *tree, int *accesosBusqueda, int *accesosInsercion);

#endif