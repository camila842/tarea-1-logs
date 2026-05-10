#include "stdio.h"
#include "stdlib.h"
#include "include/bulk-loading.h"

/*
 * Lee n pares de floats (x, y) del archivo binario in_file y los escribe en out_file
 * como estructuras Pair, donde key.x1==key.x2==x y key.y1==key.y2==y (punto como MBR degenerado)
 * y value==-1 (marca de hoja).
 * @param in_file   Ruta del archivo de entrada con pares float crudos.
 * @param out_file  Ruta del archivo de salida en formato Pair.
 * @param n         Cantidad de pares a convertir.
 * @return          0 si la conversión fue exitosa, -1 ante cualquier error de E/S.
 */
int convertir_n(const char* in_file, const char* out_file, size_t n) {
    FILE* in = fopen(in_file, "rb");
    FILE* out = fopen(out_file, "wb");

    if (!in || !out) {
        perror("Error abriendo archivos");
        return -1;
    }

    float x, y;
    Pair p;

    for (size_t i = 0; i < n; i++) {
        size_t r1 = fread(&x, sizeof(float), 1, in);

        if (r1 != 1) {
            if (feof(in)) {
                fprintf(stderr, "EOF inesperado en x (i=%zu)\n", i);
            } else {
                perror("Error leyendo x");
            }
            fclose(in);
            fclose(out);
            return -1;
        }

        size_t r2 = fread(&y, sizeof(float), 1, in);

        if (r2 != 1) {
            if (feof(in)) {
                fprintf(stderr, "Archivo inconsistente: falta y (i=%zu)\n", i);
            } else {
                perror("Error leyendo y");
            }
            fclose(in);
            fclose(out);
            return -1;
        }

        // Construir Pair
        p.key.x1 = p.key.x2 = x;
        p.key.y1 = p.key.y2 = y;
        p.value = -1;

        if (fwrite(&p, sizeof(Pair), 1, out) != 1) {
            perror("Error escribiendo");
            fclose(in);
            fclose(out);
            return -1;
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}
int main(int argc, char* argv[]){
    unsigned long total = 1 << 24;
    return convertir_n(argv[1],argv[2],total);
}