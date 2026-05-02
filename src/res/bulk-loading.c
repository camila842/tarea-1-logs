#include "include/bulk-loading.h"

int groupsNumber(double N, double b){
    int res = fmod(N,b) == 0 ? N/b : N/b + 1;
    return res;
}

float getMiddleX(Pair *tuple){
    return (tuple->key->x1 + tuple->key->x2)/(float)2;
}
float getMiddleY(Pair *tuple){
    return (tuple->key->y1 + tuple->key->y2)/(float)2;
}

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

void minMaxY(Pair *tuples, int n, int *minMax){ //falta hacer función aquí
    int *res = minMax;
    int min;
    int max;
    for(int i=0; i<n; i++){
        int y10 = tuples[i].key->y1;
        int y20 = tuples[i].key->y2;
        int minTuple = y10 <= y20 ? y10 : y20;
        int maxTuple = y10 <= y20 ? y20 : y10;

        min = minTuple <= min ? minTuple : min;
        max = maxTuple <= max ? maxTuple : max;
    }
    res[0] = min;
    res[1] = max;
}

void mbrNearestX(Node *nodo, Rectangle *mbr){
    if (nodo->k == 0){
        return;
    }
    //Dado que estarán ordenados, y la forma en que se van guardando
    //(se agrupan desde un arrreglo ordenado lo que hace que recursivamente no se superpongan)
    //solo es necesario buscar en el primero y último.
    int x1Min = nodo->hijos[0].key->x1;
    int x2Max = nodo->hijos[nodo->k - 1].key->x1;

    int minMax[2] = {0};
    minMaxY(nodo->hijos,nodo->k,minMax);
    int y1Min = minMax[0]; 
    int y2Max = minMax[1];

    Rectangle *rectangle = mbr;
    rectangle->x1 = x1Min;
    rectangle->x2 = x2Max;
    rectangle->y1 = y1Min;
    rectangle->y2 = y2Max;
}

void mbrSTR(Node *nodo, Rectangle *mbr){
    if (nodo->k == 0){
        return;
    }
    //Dado que estarán ordenados, y la forma en que se van guardando
    //(se agrupan desde un arrreglo ordenado lo que hace que recursivamente no se superpongan)
    //solo es necesario buscar en el primero y último.
    int x1Min = nodo->hijos[0].key->x1;
    int x2Max = nodo->hijos[nodo->k - 1].key->x1;
    int y1Min = nodo->hijos[0].key->y1; 
    int y2Max = nodo->hijos[nodo->k - 1].key->y2;

    Rectangle *rectangle = mbr;
    rectangle->x1 = x1Min;
    rectangle->x2 = x2Max;
    rectangle->y1 = y1Min;
    rectangle->y2 = y2Max;
}

void nearestX(Pair *parLlaveValor, int n, RTree **rtree){
    //centersArray def
    CenterRectagle array[n];
    
    //sort
    Pair *sortedArray = arrayAccorddingToCenters(parLlaveValor,array,n,getMiddleX);
    int groupsAmount = groupsNumber(n,B);

    Pair newChildrenArray[groupsAmount];
    
    RTree *tree = *rtree;
    //Pair *recRoot;
    for(int i = 0; i < groupsAmount; i++){
        int start = i * B;
        int end = (i+1)*B > n ? n : (i+1)*B;
        Node *nodo = createNode(sortedArray + start,end - start);

        int nodePosition = addNodeTotree(rtree,nodo);
        
        Rectangle mbr[1];
        mbrNearestX(nodo,mbr);
        newChildrenArray[i].key = mbr;
        newChildrenArray[i].value = nodePosition;
    }
    if(groupsAmount <= B){
        Node *rootNode = createNode(newChildrenArray,groupsAmount);
        *(tree->root) = *rootNode;
    }
    else{
        nearestX(newChildrenArray,groupsAmount,rtree);
    }

}

int separateInGroupsOf(Pair *group, Pair *elements, int groupElements, int totalElements){
    Pair *groupArray = group;
    for(int i=0; i<groupElements; i++){
        groupArray[i] = elements[i];
    }
    return totalElements - groupElements;
}

void groups(int n, int elementsPerGroup, int *results){
    int *array = results;
    int res = 0;
    int s = n/elementsPerGroup;
    int resto = n - s*elementsPerGroup;
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

void sortTileRecursive(Pair *parLlaveValor, int n, RTree **rtree){
    CenterRectagle array[n];
    Pair *sortedArray = arrayAccorddingToCenters(parLlaveValor,array,n,getMiddleX);
    //sort
    
    int S = ceil(sqrt((float)n / B));
    int elementsPerGroup = sqrt(n*B);

    int leftElements = n;
    int groupNumber = 0;

    int groupsValues[3];
    groups(n,elementsPerGroup,groupsValues);

    int s = groupsValues[0];
    int s2 = groupsValues[1];
    int totalNodes = groupsValues[2];

    int pairsPut = 0;
    Pair newPairs[totalNodes];

    RTree *tree = *rtree;


    //for(int i=0; i<groupsAmount; i++)
    //while (leftElements>0){
    for (int i=0; i<s; i++){
        
        int start = i * elementsPerGroup;
        int end = (i+1)*elementsPerGroup > n ? n : (i+1)*elementsPerGroup;
        Pair *beginningOfGroup = sortedArray + start;

        int elementsInThisGroup = end - start;
        Pair grupo[elementsPerGroup];
        leftElements = separateInGroupsOf(grupo,beginningOfGroup,elementsInThisGroup,leftElements);

        CenterRectagle centersY[elementsInThisGroup];
        Pair *sortedArrayGroup = arrayAccorddingToCenters(grupo,centersY,elementsInThisGroup,getMiddleY);
        //sort
        

        //nuevo grupo
        int elementsPerNewGroup = B;
        int newLeftElements = elementsInThisGroup;
        // int newGroupNumber = 0;
        int iterationsGroup;
        if (elementsInThisGroup ==  elementsPerGroup){
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
            Pair *beginningOfNewGroup = grupo + start;
            newLeftElements = separateInGroupsOf(nuevoGrupo,beginningOfGroup,elementsPerNewGroup,newLeftElements);

            //Node *nodo = createNode(sortedArray + i*B*sizeof(Pair),endOfGroup - i*B);
            Node *nodo = createNode(nuevoGrupo,elementsInNode);
            int nodePosition = addNodeTotree(rtree,nodo);
            
            Rectangle mbr[1];
            mbrSTR(nodo,mbr);
            newPairs[pairsPut].key = mbr;
            newPairs[pairsPut].value = nodePosition;
            pairsPut++;
        }
    }
    if(totalNodes <= B){
        Node *rootNode = createNode(newPairs,totalNodes);
        *(tree->root) = *rootNode;
    }
    else{
        sortTileRecursive(newPairs,totalNodes,rtree);
    }
}
