#include "src/include/bulk-loading.h"

/*
 * Calcula la cantidad de grupos de tamaño b necesarios para cubrir N elementos.
 * Equivalente a ceil(N/b) usando aritmética entera.
 * @param N  Cantidad total de elementos.
 * @param b  Tamaño máximo de cada grupo.
 * @return   Cantidad de grupos (último puede ser incompleto).
 */
int groupsNumber(double N, double b){
    int res = fmod(N,b) == 0 ? N/b : N/b + 1;
    return res;
}

/*
 * Retorna la coordenada X del centroide del MBR de tuple.
 * @param tuple  Par cuyo centroide se quiere calcular.
 * @return       Valor (x1+x2)/2 del rectángulo clave.
 */
float getMiddleX(Pair *tuple){
    return (tuple->key.x1 + tuple->key.x2)/(float)2;
}
/*
 * Retorna la coordenada Y del centroide del MBR de tuple.
 * @param tuple  Par cuyo centroide se quiere calcular.
 * @return       Valor (y1+y2)/2 del rectángulo clave.
 */
float getMiddleY(Pair *tuple){
    return (tuple->key.y1 + tuple->key.y2)/(float)2;
}

/*
 * Construye el arreglo centersArray asociando cada Pair con su centroide según func,
 * ordena ambos arreglos en paralelo por el valor del centroide y retorna parLlaveValor reordenado.
 * @param parLlaveValor  Arreglo de Pairs a ordenar (se reordena in-place).
 * @param centersArray   Buffer auxiliar de tamaño n para los CenterRectagle.
 * @param n              Cantidad de elementos.
 * @param func           Función que extrae el valor de centroide de un Pair (getMiddleX o getMiddleY).
 * @return               Puntero a parLlaveValor ya ordenado.
 */
Pair *arrayAccorddingToCenters(Pair *parLlaveValor, CenterRectagle *centersArray, int n, float (*func)(Pair *)){
    CenterRectagle *array = centersArray;

    for(int i = 0; i < n; i++){
        CenterRectagle element = {
            func(&parLlaveValor[i]),   
            &parLlaveValor[i]
        };
        array[i] = element;
    }
    quicksort_seq(array,0,n-1,parLlaveValor);
    
    return parLlaveValor;
}

/*
 * Calcula el mínimo y máximo de las coordenadas Y entre n Pairs.
 * @param tuples  Arreglo de Pairs a analizar.
 * @param n       Cantidad de elementos.
 * @param minMax  Arreglo de salida de tamaño 2: minMax[0]=min Y, minMax[1]=max Y.
 */
void minMaxY(Pair *tuples, int n, float *minMax){
    float *res = minMax;
    float min = 1;
    float max = 0;
    for(int i=0; i<n; i++){
        float y10 = tuples[i].key.y1;
        float y20 = tuples[i].key.y2;
        float minTuple = y10 <= y20 ? y10 : y20;
        float maxTuple = y10 <= y20 ? y20 : y10;

        min = minTuple <= min ? minTuple : min;
        max = maxTuple >= max ? maxTuple : max;
    }
    res[0] = min;
    res[1] = max;
}

/*
 * Calcula el mínimo y máximo de las coordenadas X entre n Pairs.
 * @param tuples  Arreglo de Pairs a analizar.
 * @param n       Cantidad de elementos.
 * @param minMax  Arreglo de salida de tamaño 2: minMax[0]=min X, minMax[1]=max X.
 */
void minMaxX(Pair *tuples, int n, float *minMax){
    float *res = minMax;
    float min = 1;
    float max = 0;
    for(int i=0; i<n; i++){
        float x10 = tuples[i].key.x1;
        float x20 = tuples[i].key.x2;
        float minTuple = x10 <= x20 ? x10 : x20;
        float maxTuple = x10 <= x20 ? x20 : x10;

        min = minTuple <= min ? minTuple : min;
        max = maxTuple >= max ? maxTuple : max;
    }
    res[0] = min;
    res[1] = max;
}
/*
 * Calcula el MBR del nodo asumiendo que sus hijos están ordenados por X (invariante NearestX).
 * Aprovecha el orden para obtener x1Min y x2Max del primer y último hijo, y busca min/max Y
 * recorriendo todos los hijos.
 * @param nodo  Nodo del que se calcula el MBR.
 * @param mbr   Rectángulo de salida donde se escribe el resultado.
 */
void mbrNearestX(Node *nodo, Rectangle *mbr){
    if (nodo->k == 0){
        return;
    }
    //Dado que estarán ordenados, y la forma en que se van guardando
    //(se agrupan desde un arrreglo ordenado lo que hace que recursivamente no se superpongan)
    //solo es necesario buscar en el primero y último.
    float x1Min = nodo->hijos[0].key.x1;
    float x2Max = nodo->hijos[nodo->k - 1].key.x2;

    float minMax[2] = {0};
    minMaxY(nodo->hijos,nodo->k,minMax);
    float y1Min = minMax[0]; 
    float y2Max = minMax[1];

    Rectangle *rectangle = mbr;
    rectangle->x1 = x1Min;
    rectangle->x2 = x2Max;
    rectangle->y1 = y1Min;
    rectangle->y2 = y2Max;
}

/*
 * Calcula el MBR del nodo escaneando todos sus hijos para encontrar los extremos reales.
 * Necesario porque ordenar por centroide Y no garantiza que y1[0] sea el mínimo ni que
 * y2[last] sea el máximo cuando los hijos son MBRs de nodos internos (rectángulos con extensión).
 * @param nodo  Nodo del que se calcula el MBR.
 * @param mbr   Rectángulo de salida donde se escribe el resultado.
 */
void MBR(Node *nodo, Rectangle *mbr){
    if (nodo->k == 0){
        return;
    }
    float rangeX[2] = {0};
    float rangeY[2] = {0};
    minMaxX(nodo->hijos, nodo->k, rangeX);
    minMaxY(nodo->hijos, nodo->k, rangeY);

    Rectangle *rectangle = mbr;
    rectangle->x1 = rangeX[0];
    rectangle->x2 = rangeX[1];
    rectangle->y1 = rangeY[0];
    rectangle->y2 = rangeY[1];
}

/*
 * Construye un R-Tree mediante el algoritmo NearestX bulk-loading.
 * Ordena los n pares por la coordenada X del centroide, los agrupa en hojas de
 * tamaño B y construye los niveles superiores de forma recursiva hasta la raíz.
 * @param parLlaveValor  Arreglo de n Pairs (puntos o MBRs) a insertar.
 * @param n              Cantidad de elementos en parLlaveValor.
 * @param rtree          R-Tree destino donde se escribirán los nodos.
 */
void nearestX(Pair *parLlaveValor, int n, RTree *rtree){
    printf("[LOG] nearestX: nivel con %d elementos\n", n);
    //centersArray def
    CenterRectagle *array = malloc(sizeof(CenterRectagle) * n);
    if (!array) {
        printf("[LOG] ERROR: malloc falló en nearestX (array)\n");
        return;
    }

    //sort
    Pair *sortedArray = arrayAccorddingToCenters(parLlaveValor,array,n,getMiddleX);
    free(array);
    int groupsAmount = groupsNumber(n,B);
    printf("[LOG] nearestX: %d grupos (B=%d)\n", groupsAmount, B);

    Pair newChildrenArray[groupsAmount];

    //Pair *recRoot;
    for(int i = 0; i < groupsAmount; i++){
        int start = i * B;
        int end = (i+1)*B > n ? n : (i+1)*B;
        Node *nodo = createNode(sortedArray + start,end - start, rtree->tree);

        int nodePosition = addNodeTotree(rtree,nodo,NUEVO_NODO);
        printf("[LOG] nearestX: nodo hoja %d/%d creado (pos=%d, hijos=%d)\n",
               i+1, groupsAmount, nodePosition, end-start);

        Rectangle mbr;
        MBR(nodo,&mbr);
        newChildrenArray[i].key = mbr;
        newChildrenArray[i].value = nodePosition;
        free(nodo);
    }
    if(groupsAmount <= B){
        printf("[LOG] nearestX: creando raíz con %d hijos\n", groupsAmount);
        Node *rootNode = createNode(newChildrenArray,groupsAmount,rtree->tree);
        addNodeTotree(rtree,rootNode,RAIZ);
        free(rootNode);
    }
    else{
        printf("[LOG] nearestX: nivel excede B, recursión con %d nodos\n", groupsAmount);
        nearestX(newChildrenArray,groupsAmount,rtree);
    }

}

/*
 * Copia los primeros groupElements elementos de elements[] a group[].
 * @param group         Arreglo destino de tamaño ≥ groupElements.
 * @param elements      Arreglo fuente.
 * @param groupElements Cantidad de elementos a copiar.
 * @param totalElements Cantidad total de elementos restantes antes de esta copia.
 * @return              totalElements - groupElements (elementos que quedan por procesar).
 */
int separateInGroupsOf(Pair *group, Pair *elements, int groupElements, int totalElements){
    Pair *groupArray = group;
    for(int i=0; i<groupElements; i++){
        groupArray[i] = elements[i];
    }
    return totalElements - groupElements;
}

/*
 * Calcula la distribución de grupos y subgrupos para STR dado n elementos y un tamaño de franja.
 * @param n                Cantidad total de elementos.
 * @param elementsPerGroup Tamaño de cada franja horizontal (≈ sqrt(n*B)).
 * @param results          Arreglo de salida de tamaño 3:
 *                           results[0] = s  (cantidad de franjas horizontales),
 *                           results[1] = s2 (subgrupos por franja completa),
 *                           results[2] = totalNodes (nodos hoja totales que se crearán).
 */
void groups(int n, unsigned int elementsPerGroup, int *results){
    int *array = results;
    int res = 0;
    int s = n/elementsPerGroup;
    unsigned int resto = n - s*elementsPerGroup;
    int s2;
    if (resto > 0 && resto<=elementsPerGroup){
        
        s2 = elementsPerGroup/B;
        int resto2 = elementsPerGroup - s2*B;
        if (resto2 > 0 && resto2<=B){
            s2++;
            res += s*s2;
        }
        else if (resto2 == 0)
        {
            res += s*s2;
        }else{
            res = 0;
        }
        s++;
        int lastGroups = resto/B;
        int resto3 = resto - lastGroups*B;
        res += resto3 > 0 ? lastGroups + 1 : lastGroups;
    }
    else if(resto == 0){
        s2 = elementsPerGroup/B;
        int resto2 = elementsPerGroup - s2*B;
        if(resto2 == 0){
            res += s2*s;
        } else if (resto2 > 0 && resto2<=B){
            s2++;
            res += s*s2;
        }else{
            res = 0;
        }
    } else{
        s2 = 0;
        res = 0;
    }
    array[0] = s;
    array[1] = s2;
    array[2] = res;
}

/*
 * Construye un R-Tree mediante el algoritmo STR (Sort-Tile-Recursive) bulk-loading.
 * Divide los datos en S franjas ordenadas por X, luego ordena cada franja por Y
 * y las subdivide en grupos de tamaño B para formar las hojas. Recursivo hacia arriba.
 * @param parLlaveValor  Arreglo de n Pairs (puntos o MBRs) a insertar.
 * @param n              Cantidad de elementos en parLlaveValor.
 * @param rtree          R-Tree destino donde se escribirán los nodos.
 */
void sortTileRecursive(Pair *parLlaveValor, int n, RTree *rtree){
    printf("[LOG] STR: nivel con %d elementos\n", n);
    CenterRectagle *array = malloc(sizeof(CenterRectagle) * n);
    if (!array) {
        printf("[LOG] ERROR: malloc falló en nearestX (array)\n");
        return;
    }
    Pair *sortedArray = arrayAccorddingToCenters(parLlaveValor,array,n,getMiddleX);
    free(array);

    unsigned int S = ceil(sqrt((float)n / B));
    unsigned int elementsPerGroup = (unsigned int)sqrt((double)n * B);
    printf("[LOG] STR: S=%d, elementsPerGroup=%d\n", S, elementsPerGroup);

    int leftElements = n;
    //int groupNumber = 0;

    int groupsValues[3];
    groups(n,elementsPerGroup,groupsValues);

    int s = groupsValues[0];
    int s2 = groupsValues[1];
    int totalNodes = groupsValues[2];
    printf("[LOG] STR: s=%d, s2=%d, totalNodes=%d\n", s, s2, totalNodes);

    int pairsPut = 0;
    Pair *newPairs = malloc(sizeof(Pair) * totalNodes);
    if (!newPairs) {
        printf("[LOG] ERROR: malloc falló (newPairs)\n");
        return;
    }

    RTree *tree = rtree;


    //for(int i=0; i<groupsAmount; i++)
    //while (leftElements>0){
    for (int i=0; i<s; i++){
        
        unsigned int start = i * elementsPerGroup;
        unsigned int end = (i+1)*elementsPerGroup > (unsigned)n ? (unsigned)n : (unsigned)(i+1)*elementsPerGroup;
        Pair *beginningOfGroup = sortedArray + start;
        int elementsInThisGroup = end - start;
        Pair *grupo = malloc(sizeof(Pair) * elementsPerGroup);
        if (!grupo) {
            printf("[LOG] ERROR: malloc falló (grupo)\n");
            free(newPairs);
            return;
        }
        leftElements = separateInGroupsOf(grupo,beginningOfGroup,elementsInThisGroup,leftElements);

        CenterRectagle *centersY = malloc(sizeof(CenterRectagle) * elementsInThisGroup);
        if (!centersY) {
            printf("[LOG] ERROR: malloc falló (centersY)\n");
            free(grupo);
            free(newPairs);
            return;
        }
        Pair *sortedArrayGroup = arrayAccorddingToCenters(grupo,centersY,elementsInThisGroup,getMiddleY);
        free(centersY); 
        

        //nuevo grupo
        int elementsPerNewGroup = B;
        int newLeftElements = elementsInThisGroup;
        // int newGroupNumber = 0;
        int iterationsGroup;
        if ((unsigned)elementsInThisGroup ==  elementsPerGroup){
            iterationsGroup = s2;
        }else{
            int lastGroups = elementsInThisGroup/B;
            int resto = elementsInThisGroup - lastGroups*B;
            iterationsGroup = resto > 0 ? lastGroups + 1 : lastGroups;
        }

        for (int j=0; j<iterationsGroup; j++){

            Pair nuevoGrupo[B];
            int startSubGroup = j * elementsPerNewGroup;
            int endSubGroup = (j+1)*elementsPerNewGroup > elementsInThisGroup ? elementsInThisGroup : (j+1)*elementsPerNewGroup;
            int elementsInNode = endSubGroup - startSubGroup;
            Pair *beginningOfNewGroup = sortedArrayGroup + startSubGroup;
            newLeftElements = separateInGroupsOf(nuevoGrupo,beginningOfNewGroup,elementsPerNewGroup,newLeftElements);

            //Node *nodo = createNode(sortedArray + i*B*sizeof(Pair),endOfGroup - i*B);
            Node *nodo = createNode(nuevoGrupo,elementsInNode,tree->tree);
            int nodePosition = addNodeTotree(tree,nodo,NUEVO_NODO);
            printf("[LOG] STR: nodo hoja creado (grupo=%d, subgrupo=%d, pos=%d, hijos=%d)\n",
                   i+1, j+1, nodePosition, elementsInNode);

            Rectangle mbr;
            MBR(nodo,&mbr);
            newPairs[pairsPut].key = mbr;
            newPairs[pairsPut].value = nodePosition;
            pairsPut++;
            free(nodo);
        }
        free(grupo);
    }
    if(totalNodes <= B){
        printf("[LOG] STR: creando raíz con %d hijos\n", totalNodes);
        // Node *rootNode = createNode(newPairs,totalNodes);
        // *(tree->root) = *rootNode;
        Node *rootNode = createNode(newPairs,totalNodes,tree->tree);
        addNodeTotree(tree,rootNode,RAIZ);
        free(rootNode);
    }
    else{
        printf("[LOG] STR: nivel excede B, recursión con %d nodos\n", totalNodes);
        sortTileRecursive(newPairs,totalNodes,rtree);
    }
    free(newPairs);
}
