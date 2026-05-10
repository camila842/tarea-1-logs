CC = gcc
CFLAGS = -I. -Wall -Wextra -O2
LDFLAGS = -lm

# Objetos comunes
COMMON = src/res/bulk-loading.o src/res/rtree.o src/res/sort.o

# Ejecutables
CONVERT = convertir_data
ARBOL = arbolx
CONSULTAS = consultas-arbol

all: $(CONVERT) $(ARBOL) $(CONSULTAS)

# === REGLAS EXPLÍCITAS (clave) ===
src/res/bulk-loading.o: src/res/bulk-loading.c
	$(CC) $(CFLAGS) -c $< -o $@

src/res/rtree.o: src/res/rtree.c
	$(CC) $(CFLAGS) -c $< -o $@

src/res/sort.o: src/res/sort.c
	$(CC) $(CFLAGS) -c $< -o $@

src/data_conversion.o: src/data_conversion.c
	$(CC) $(CFLAGS) -c $< -o $@

src/generar_arboles.o: src/generar_arboles.c
	$(CC) $(CFLAGS) -c $< -o $@

src/consultas.o: src/consultas.c
	$(CC) $(CFLAGS) -c $< -o $@

# === LINK ===
$(CONVERT): $(COMMON) src/data_conversion.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(ARBOL): $(COMMON) src/generar_arboles.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(CONSULTAS): $(COMMON) src/consultas.o
	$(CC) -o $@ $^ $(LDFLAGS)

# === RUNS ===
run-convert: $(CONVERT)
	./$(CONVERT) europa.bin euConvertido.bin
	./$(CONVERT) random.bin ranConvertido.bin

run-arboles: $(ARBOL)
	./$(ARBOL) ranConvertido.bin nearestX random
	./$(ARBOL) euConvertido.bin nearestX europa
	./$(ARBOL) ranConvertido.bin str random
	./$(ARBOL) euConvertido.bin str europa

run-consultas: $(CONSULTAS)
	./$(CONSULTAS) arboles/arbol_random_str_24.bin
	./$(CONSULTAS) arboles/arbol_europa_str_24.bin
	./$(CONSULTAS) arboles/arbol_random_nrstX_24.bin
	./$(CONSULTAS) arboles/arbol_europa_nrstX_24.bin

run-all: run-convert run-arboles run-consultas

clean:
	rm -f src/*.o src/res/*.o $(CONVERT) $(ARBOL) $(CONSULTAS)

.PHONY: all clean run-convert run-arboles run-consultas run-all