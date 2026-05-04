//#include "include/rtree.h"
#include "src/include/rtree.h"


Rectangle* createRectangle(float x1, float x2, float y1, float y2) {
    Rectangle *r = (Rectangle*) malloc(sizeof(Rectangle));
    if (!r) return NULL;

    r->x1 = x1;
    r->x2 = x2;
    r->y1 = y1;
    r->y2 = y2;

    return r;
}
int savePair(Pair *tuple, FILE *f){
    if (fwrite(&(tuple->key), sizeof(Rectangle), 1, f) != 1) return FALSE;
    if (fwrite(&(tuple->value), sizeof(int), 1, f) != 1) return FALSE;
    return TRUE;
}
int parsePair(Pair *tuple, FILE *f){
    if (fread(&(tuple->key), sizeof(Rectangle), 1, f) != 1) return FALSE;
    if (fread(&(tuple->value), sizeof(int), 1, f) != 1) return FALSE;
    return TRUE;
}

int saveNode(Node *nodo, FILE *f){
    if (fwrite(&(nodo->k), sizeof(int), 1, f) != 1) return FALSE;
    for (int i = 0; i < B; i++) {
        if (!savePair(&(nodo->hijos[i]), f)) return FALSE;
    }
    if (fwrite(nodo->pad, 1, 12, f) != 12) return FALSE;
    return TRUE;
}

int parseNode(Node *nodo, FILE *f, int pos){
    int sizeRect = sizeof(Rectangle);
    int sizePair = sizeof(int) + sizeRect;
    int sizeNode = sizeof(int) + sizePair*B + 12;
    if (fseek(f, pos*sizeNode, SEEK_SET) != 0) return FALSE;
    if (fread(&(nodo->k), sizeof(int), 1, f) != 1) return FALSE;
    for (int i = 0; i < B; i++) {
        if (!parsePair(&(nodo->hijos[i]), f)) return FALSE;
    }
    if (fread(nodo->pad, 1, 12, f) != 12) return FALSE;
    return TRUE;
}

Node *createNode(Pair *children, int n, FILE *f) {
    Node node = {0};
    if(children == NULL){
        node.k = 0;
        // Inicializar hijos
        for (int i = 0; i < B; i++) {
            Rectangle key = {0};
            node.hijos[i].key = key;
            node.hijos[i].value = -1;
        }
    }
    else {
        node.k = n;
        addChildrenToNode(&node,children,n);
    }
    //saveNode(&node,f);
    Node *pointer = (Node *)malloc(sizeof(Node));
    *pointer = node;
    return pointer;
}

RTree* createRTree(char *name) {
    RTree tree = {0};
    tree.nodes = 0;
    //tree->root[0] = *(createNode(NULL,0));
    printf("[LOG] createRTree: abriendo '%s'\n", name);
    tree.tree = fopen(name,"wb+");
    if (!tree.tree) {
        printf("[LOG] createRTree: ERROR al abrir '%s'\n", name);
        return NULL;
    }
    Node root = {0};
    if (!saveNode(&root, tree.tree)) {
        fclose(tree.tree);
        return NULL;
    }
    RTree *rtree = (RTree*) malloc(sizeof(RTree));
    if (!rtree) {
        fclose(tree.tree);
        return NULL;
    }
    *rtree = tree;

    return rtree;
}

void addChildrenToNode(Node *node, Pair *children, int n){
    Node *nodo = node;
    if(n <= B){
        for (int i = 0; i < n; i++){
            nodo->hijos[i] = children[i];
        }
    }
}

int addNodeTotree(RTree *tree,Node *node, Insercion insercion){
    RTree *rtree = tree;
    rtree->nodes += 1;
    int seekResult;
    switch (insercion)
    {
    case RAIZ:
        printf("[LOG] addNodeTotree: escribiendo RAIZ (total nodos: %d)\n", rtree->nodes);
        seekResult = fseek(rtree->tree, 0, SEEK_SET);
        break;
    case NUEVO_NODO:
        seekResult = fseek(rtree->tree, 0, SEEK_END);
        break;
    default:
        return -1;
    }
    if (seekResult != 0) return -1;
    if (!saveNode(node, rtree->tree)) return -1;
    return rtree->nodes;
}

void freeRTree(RTree *tree) {
    if (tree == NULL) return;
    fclose(tree->tree);
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

void interseccion(Rectangle *rect, Node *nodo, FILE *puntos, int *cantidadPuntos, RTree *tree){
    if(nodo == NULL){
        return; 
    }
    //Pair *rectangulos = nodo->hijos;
    for(int i=0; i<nodo->k; i++){
        Rectangle *thisRectangle = &(nodo->hijos[i].key);
        int pairValue = nodo->hijos[i].value;
        if(seIntersectan(rect,thisRectangle)){
            if(pairValue == -1){
                // puntos[*cantidadPuntos] = *thisRectangle;
                // *(cantidadPuntos)++;
                fseek(puntos,0,SEEK_END);
                fwrite(&(thisRectangle->x1),sizeof(float),1,puntos);
                fwrite(&(thisRectangle->y1),sizeof(float),1,puntos);        
                return ;
            }
            Node nodoRecursion;
            if (!parseNode(&nodoRecursion, tree->tree, pairValue)) return;
            interseccion(rect,&nodoRecursion,puntos,cantidadPuntos,tree);
        }else{
            return;
        }
    }
}