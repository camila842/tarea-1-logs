#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "include/bulk-loading.h"


/*
 * Genera un rectángulo cuadrado de lado s con esquina inferior izquierda aleatoria uniforme,
 * garantizando que el rectángulo quede completamente dentro del espacio [0,1]².
 * @param s     Longitud del lado del rectángulo (0 < s < 1).
 * @return      Puntero al Rectangle creado en el heap, o NULL si falla malloc.
 */
Rectangle *generateRectangle(double s){
    double x1 = (double)rand() / RAND_MAX * ((double)1-s);
    double y1 = (double)rand() / RAND_MAX * ((double)1-s);
    Rectangle *rectangle = (Rectangle *)malloc(sizeof(Rectangle));
    if (!rectangle) {
        printf("[LOG] generateRectangle: ERROR malloc fallido\n");
        return NULL;
    }
    rectangle->x1 = x1;
    rectangle->x2 = x1 + s;
    rectangle->y1 = y1;
    rectangle->y2 = y1 + s;
    return rectangle;
}


/*
 * Escribe una fila de métricas en el archivo CSV f con el resultado de una consulta.
 * @param f                Archivo CSV de métricas ya abierto para escritura.
 * @param puntosContenidos Cantidad de puntos encontrados dentro del rectángulo.
 * @param lecturas         Cantidad de accesos a disco realizados durante la búsqueda.
 * @param s                Longitud del lado del rectángulo de consulta.
 * @param attempt          Número de intento (0-99) dentro del lote de 100 consultas.
 * @param arbol            Nombre del archivo del árbol consultado.
 */
void reportMetrics(FILE *f, int puntosContenidos, int lecturas, double s, int attempt, char *arbol ){
    char fila[100];
    snprintf(fila,sizeof(fila),"%f,%d,%d,%d,%s\n",s,attempt,puntosContenidos,lecturas,arbol);
    size_t len = strlen(fila);
    if (fwrite(fila,1,len,f) != len) {
        printf("[LOG] reportMetrics: ERROR escribiendo fila de métricas\n");
    }
}

/*
 * Calcula la cantidad de puntos (x,y) almacenados en el archivo binario f.
 * Cada punto ocupa dos floats (8 bytes), por lo que la cantidad es tamaño/(2*sizeof(float)).
 * @param f  Handle del archivo binario de resultados de una consulta.
 * @return   Cantidad de puntos contenidos en el archivo.
 */
int calculateAMountofDots(FILE *f){
    fseek(f,0,SEEK_END);
    int size = ftell(f);
    int amount = (size/sizeof(float))/2;
    return amount;
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("Uso: %s <archivo_datos>\n", argv[0]);
        return 1;
    }

    char *arbol = argv[1];
    printf("[LOG] main: abriendo árbol '%s'\n", arbol);

    // Abrir en modo lectura para no truncar el árbol ya construido
    FILE *treeFile = fopen(arbol, "rb");
    if (!treeFile) {
        printf("[LOG] main: ERROR no se pudo abrir '%s'\n", arbol);
        return 1;
    }

    RTree *tree = (RTree *)malloc(sizeof(RTree));
    if (!tree) {
        printf("[LOG] main: ERROR malloc para RTree\n");
        fclose(treeFile);
        return 1;
    }
    tree->tree = treeFile;
    tree->nodes = 0;

    Node nodo;
    if (!parseNode(&nodo, tree->tree, 0)) {
        printf("[LOG] main: ERROR al parsear nodo raíz del árbol\n");
        fclose(treeFile);
        free(tree);
        return 1;
    }
    printf("[LOG] main: raíz cargada (k=%d hijos)\n", nodo.k);

    FILE *metrics = fopen("arboles/metricas.txt", "a+");
    if (!metrics) {
        printf("[LOG] main: ERROR no se pudo abrir metricas.txt\n");
        fclose(treeFile);
        free(tree);
        return 1;
    }

    fseek(metrics, 0, SEEK_END);
    long size = ftell(metrics);
    if (size == 0) {
        fprintf(metrics, "largo_rectangulo,Intento,puntos,lecturas,arbol\n");
        fflush(metrics);
    }

    system("if not exist consultas mkdir consultas");

    double s[5] = {0.0025, 0.005, 0.01, 0.025, 0.05};
    for(int i=0; i<5; i++){
        printf("[LOG] main: s=%.4f — iniciando 100 consultas\n", s[i]);
        for(int j=0; j<100; j++){
            Rectangle *thisRect = generateRectangle(s[i]);
            if (!thisRect) {
                printf("[LOG] main: ERROR generando rectángulo (i=%d, j=%d), saltando\n", i, j);
                continue;
            }

            char filename[50];
            snprintf(filename,sizeof(filename),"arboles/resultado_s-%d_%d.bin",i,j);
            FILE *f = fopen(filename,"wb");
            if (!f) {
                printf("[LOG] main: ERROR abriendo '%s' para escritura (i=%d, j=%d), saltando\n", filename, i, j);
                free(thisRect);
                continue;
            }

            int accesos = 1;
            int escrito = 0;
            interseccion(thisRect,&nodo,f,tree,&accesos,&escrito);

            int puntosEncontrados = calculateAMountofDots(f);
            reportMetrics(metrics,puntosEncontrados,accesos,s[i],j,arbol);
            fclose(f);
            remove(filename);
            free(thisRect);
        }
        printf("[LOG] main: s=%.4f — completado\n", s[i]);
    }

    fclose(metrics);
    fclose(treeFile);
    free(tree);
    return 0;
}
