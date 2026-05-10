#include <stdio.h>
#include <stdlib.h>

#define B 204

typedef struct {
    float x1, x2, y1, y2;
} Rectangle;

typedef struct {
    Rectangle key;
    int value;
} Pair;

// Tamaño fijo de un nodo en disco: int k + B pares + 12 bytes pad
#define SIZE_NODE (sizeof(int) + (size_t)B * sizeof(Pair) + 12)

void leerNodo(FILE *f, int posicion) {
    if (fseek(f, (long)posicion * SIZE_NODE, SEEK_SET) != 0) {
        printf("Error en fseek\n");
        return;
    }

    int k;
    if (fread(&k, sizeof(int), 1, f) != 1) {
        printf("Error leyendo k\n");
        return;
    }

    printf("Nodo en posición %d\n", posicion);
    printf("k = %d\n", k);

    for (int i = 0; i < B; i++) {
        Pair p;
        if (fread(&p, sizeof(Pair), 1, f) != 1) {
            printf("Error leyendo Pair %d\n", i);
            return;
        }

        if (i < k) {
            printf("  Hijo %d:\n", i);
            printf("    Rect: [%.4f %.4f] x [%.4f %.4f]\n",
                   p.key.x1, p.key.x2, p.key.y1, p.key.y2);
            printf("    value: %d\n", p.value);
        }
    }

    char pad[12];
    if (fread(pad, 1, 12, f) != 12) {
        printf("Error leyendo pad\n");
        return;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <archivo> <posicion>\n", argv[0]);
        return 1;
    }

    char *archivo = argv[1];
    int posicion = atoi(argv[2]);

    FILE *f = fopen(archivo, "rb");
    if (!f) {
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

    leerNodo(f, posicion);

    fclose(f);
    return 0;
}
