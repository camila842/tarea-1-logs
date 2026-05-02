//#include "include/rtree.h"
#include "include/rtree.h"


Rectangle* createRectangle(float x1, float x2, float y1, float y2) {
    Rectangle *r = (Rectangle*) malloc(sizeof(Rectangle));
    if (!r) return NULL;

    r->x1 = x1;
    r->x2 = x2;
    r->y1 = y1;
    r->y2 = y2;

    return r;
}

Node* createNode(Pair *children, int n) {
    Node *node = (Node*) malloc(sizeof(Node));
    if (!node) return NULL;

    if(children == NULL){
        node->k = 0;

        // Inicializar hijos
        for (int i = 0; i < B; i++) {
            node->hijos[i].key = NULL;
            node->hijos[i].value = -1;
        }
    }
    else {
        node->k = n;
        addChildrenToNode(&node,children,n);
    }
    return node;
}

RTree* createRTree() {
    RTree *tree = (RTree*) malloc(sizeof(RTree));
    if (!tree) return NULL;
    tree->nodes = 0;
    tree->root = createNode(NULL,0);

    return tree;
}

void addChildrenToNode(Node **node, Pair *children, int n){
    Node *nodo = *node;
    if(n <= B){
        for (int i = 0; i < n; i++){
            nodo->hijos[i] = children[i];
        }
    }
}

int addNodeTotree(RTree **tree,Node *node){
    RTree *rtree = *tree;
    rtree->nodes += 1; 
    *(rtree->root + rtree->nodes*sizeof(Node)) = *node;
    return rtree->nodes;
}

void freeNode(Node *node) {
    if (node == NULL) return;

    // liberar solo los usados (0 a k-1)
    for (int i = 0; i < node->k; i++) {
        if (node->hijos[i].key != NULL) {
            free(node->hijos[i].key);
        }
    }

    free(node);
}

void freeRTree(RTree *tree) {
    if (tree == NULL) return;

    freeNode(tree->root);
    free(tree);
}

int seIntersectan(Rectangle *r1, Rectangle *r2) {
    // Separación en X
    if (r1->x2 < r2->x1 || r2->x2 < r1->x1)
        return FALSE;

    // Separación en Y
    if (r1->y2 < r2->y1 || r2->y2 < r1->y1)
        return FALSE;

    return TRUE;
}

void interseccion(Rectangle *rect, Node *nodo, Rectangle *puntos, int *cantidadPuntos, RTree *tree){
    if(nodo == NULL){
        return; 
    }
    //Pair *rectangulos = nodo->hijos;
    for(int i=0; i<nodo->k; i++){
        Rectangle *thisRectangle = nodo->hijos[i].key;
        int pairValue = nodo->hijos[i].value;
        if(seIntersectan(rect,thisRectangle)){
            if(pairValue == -1){
                puntos[*cantidadPuntos] = *thisRectangle;
                *(cantidadPuntos)++;
                return ;
            }
            interseccion(rect,&(tree->root[pairValue]),puntos,cantidadPuntos,tree);
        }else{
            return;
        }
    }
}