#include "include/bulk-loading.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
//Sección de experimentación

/*
 * Lee cantidadDatos pares desde el archivo binario filename y los almacena en puntos[].
 * @param puntos        Arreglo destino donde se guardan los Pairs leídos.
 * @param filename      Ruta del archivo binario de entrada en formato Pair.
 * @param cantidadDatos Cantidad de pares a leer.
 */
void readPairs(Pair puntos[], char *filename, int cantidadDatos){
    printf("[LOG] Abriendo archivo de datos: %s\n", filename);
    FILE *f = fopen(filename,"rb+");
    if (!f) {
        printf("[LOG] ERROR: no se pudo abrir '%s'\n", filename);
        return;
    }
    int i = 0;
    while(i < cantidadDatos){
        Pair tuple;
        if(parsePair(&tuple,f)){
            puntos[i] = tuple;
            i++;
        }
    }
    fclose(f);
    printf("[LOG] Lectura completada: %d pares cargados\n", i);
}

/*
 * Ejecuta un experimento de construcción de R-Tree con 2^exp puntos usando el algoritmo func.
 * Crea el árbol en disco, mide el tiempo de construcción y registra el resultado en log_tiempos.
 * @param exp           Exponente del tamaño del experimento; se usarán 2^exp puntos.
 * @param archivo_datos Ruta del archivo Pair convertido con los datos de entrada.
 * @param log_tiempos   Handle del archivo CSV donde se escribe el tiempo medido (puede ser NULL).
 * @param func          Función de bulk-loading a usar (nearestX o sortTileRecursive).
 * @param nombre_arbol  Prefijo del nombre de archivo del árbol generado (p.ej. "arbol_random_nrstX").
 * @return              0 si el experimento fue exitoso, 1 ante cualquier error.
 */
int run_experiment(int exp, char *archivo_datos, FILE *log_tiempos, void (*func)(Pair *, int, RTree *),char *nombre_arbol) {

    printf("[LOG] === Inicio exp=%d ===\n", exp);

    int datosAUtilizar = 1 << exp;
    printf("[LOG] exp=%d  ->  datos a utilizar: %d\n", exp, datosAUtilizar);

    clock_t inicio, fin;
    inicio = clock();

    char filename[64];
    snprintf(filename, sizeof(filename), "arboles/%s_%d.bin", nombre_arbol, exp);

    printf("[LOG] Creando RTree en archivo: %s\n", filename);

    RTree *arbol = createRTree(filename);
    if (!arbol) {
        printf("[LOG] ERROR: no se pudo crear el RTree\n");
        return 1;
    }

    Pair *puntos = malloc(sizeof(Pair) * datosAUtilizar);
    if (!puntos) {
        printf("[LOG] ERROR: malloc falló para puntos\n");
        freeRTree(arbol);
        return 1;
    }

    printf("[LOG] Leyendo datos desde: %s\n", archivo_datos);
    readPairs(puntos, archivo_datos, datosAUtilizar);

    printf("[LOG] Iniciando NearestX bulk-loading con %d elementos...\n", datosAUtilizar);
    func(puntos, datosAUtilizar, arbol);

    printf("[LOG] NearestX completado. Total nodos en árbol: %d\n", arbol->nodes);

    fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

    printf("[LOG] Tiempo exp=%d: %f segundos\n", exp, tiempo);

    if (log_tiempos) {
        fprintf(log_tiempos, "%s,%d,%.9f\n", nombre_arbol, datosAUtilizar, tiempo);
        fflush(log_tiempos);
    }

    free(puntos);
    freeRTree(arbol);

    printf("[LOG] === Fin exp=%d ===\n\n", exp);

    return 0;
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Uso: %s <archivo_datos>\n", argv[0]);
        return 1;
    }

    char *datos = argv[1];
    char *fuente = argv[3];

    //choosing function
    Function funct;
    if(strcmp(argv[2],"nearestX") == 0){
        funct = NEARESTX;
    }
    else if(strcmp(argv[2],"str") == 0){
        funct = STRECURSIVE;
    }
    else{
        printf("Funcion %s no valida\n", argv[2]);
        return 1;
    }

    FILE *log_tiempos = fopen("arboles/tiempos.txt", "a+");
    if (!log_tiempos) {
        printf("[LOG] ERROR: no se pudo abrir tiempos.txt\n");
        return 1;
    }

    fseek(log_tiempos, 0, SEEK_END);
    long size = ftell(log_tiempos);
    if (size == 0) {
        fprintf(log_tiempos, "archivo,datos,tiempo\n");
        fflush(log_tiempos);
    }

    for (int exp = 15; exp <= 24; exp++) {

        int experiment; 
        char arbol[50];
        switch (funct)
        {
        case STRECURSIVE:
            sprintf(arbol,"arbol_%s_str",fuente);
            experiment = run_experiment(exp, datos, log_tiempos, sortTileRecursive, arbol);
            break;

        case NEARESTX:
            sprintf(arbol,"arbol_%s_nrstX",fuente);
            experiment = run_experiment(exp, datos, log_tiempos, nearestX, arbol);
            break;
        
        default:
            experiment = -1;
            break;
        }
        
        if (experiment != 0) {
            printf("[LOG] ERROR en exp=%d\n", exp);
        }
    }

    fclose(log_tiempos);
    return 0;
}