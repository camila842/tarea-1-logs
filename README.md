# tarea-1-logs

Implementación y comparación experimental de dos algoritmos de *bulk-loading* para R-Trees sobre conjuntos de datos geoespaciales 2D:

- **NearestX**: ordena los puntos por la coordenada X del centroide y los agrupa secuencialmente en hojas.
- **STR (Sort-Tile-Recursive)**: divide los datos en franjas horizontales y luego verticales de forma recursiva para lograr mayor solapamiento mínimo entre nodos.

Se mide el tiempo de construcción (experimentos de 2¹⁵ a 2²⁴ puntos) y el costo de consultas de intersección (número de accesos a disco y puntos encontrados).

---

## Requisitos

| Herramienta | Versión mínima | Instalación |
|---|---|---|
| GCC | 7 | ver abajo |
| GNU Make | 4 | ver abajo |

```bash
# Ubuntu / Debian
sudo apt install build-essential

# macOS (con Homebrew)
brew install gcc make
```

En **Windows** se recomienda usar **WSL** (Windows Subsystem for Linux) o **MinGW-w64**.  
Para instalar WSL: `wsl --install` en PowerShell como administrador.

> Las bibliotecas `libc` y `libm` son parte de la instalación estándar de GCC y no requieren instalación adicional.

---

## Archivos de datos requeridos

Los siguientes archivos binarios son provistos por el equipo docente y deben colocarse en el **directorio raíz** del proyecto antes de ejecutar:

| Archivo | Contenido |
|---|---|
| `europa.bin` | Coordenadas geoespaciales reales (Europa) |
| `random.bin` | Coordenadas aleatorias uniformes |

Cada archivo contiene pares de valores `float` (x, y) en formato binario little-endian.

---

## Estructura del proyecto

```
tarea-1-logs/
├── src/
│   ├── include/          # Cabeceras (.h) con definición de estructuras y prototipos
│   │   ├── rtree.h       # Estructuras y funciones del R-Tree
│   │   ├── sort.h        # Quicksort auxiliar
│   │   └── bulk-loading.h# Algoritmos NearestX y STR
│   ├── res/              # Implementaciones de las librerías (entregadas por el equipo docente)
│   │   ├── rtree.c
│   │   ├── sort.c
│   │   └── bulk-loading.c
│   ├── data_conversion.c # Paso 1: convierte datos crudos al formato interno
│   ├── generar_arboles.c # Paso 2: construye los R-Trees
│   └── consultas.c       # Paso 3: ejecuta consultas de intersección
├── graphics/
│   └── graphics.ipynb    # Notebook de visualización de resultados
├── makefile
├── europa.bin            # (requerido, ver arriba)
└── random.bin            # (requerido, ver arriba)
```

---

## Compilación

Compila los tres ejecutables (`convertir_data`, `arbolx`, `consultas-arbol`) a partir de todos los fuentes:

```bash
make
```

---

## Ejecución paso a paso

Los pasos deben ejecutarse **en orden**: cada uno genera archivos que el siguiente necesita.

---

### Paso 1 — Convertir los datos de entrada

Convierte `europa.bin` y `random.bin` del formato crudo (`float x, float y`) al formato `Pair` binario que utilizan los algoritmos internamente:

```bash
make run-convert
```

**Genera:** `euConvertido.bin`, `ranConvertido.bin`

---

### Paso 2 — Construir los R-Trees

Construye 4 árboles (2 algoritmos × 2 conjuntos de datos), repitiendo el experimento con tamaños de 2¹⁵ a 2²⁴ puntos. Los árboles se serializan en disco dentro de la carpeta `arboles/`:

```bash
make run-arboles
```

**Genera:**
- `arboles/arbol_random_nrstX_<exp>.bin` — árbol NearestX sobre datos random (exp = 15..24)
- `arboles/arbol_europa_nrstX_<exp>.bin` — árbol NearestX sobre datos europa
- `arboles/arbol_random_str_<exp>.bin` — árbol STR sobre datos random
- `arboles/arbol_europa_str_<exp>.bin` — árbol STR sobre datos europa
- `arboles/tiempos.txt` — tiempos de construcción en CSV (`archivo,datos,tiempo`)

---

### Paso 3 — Ejecutar consultas de intersección

Para cada uno de los 4 árboles con 2²⁴ puntos, lanza 100 consultas de intersección por cada uno de los 5 tamaños de rectángulo de consulta (s ∈ {0.0025, 0.005, 0.01, 0.025, 0.05}):

```bash
make run-consultas
```

**Genera:** `arboles/metricas.txt` — métricas en CSV (`largo_rectangulo,Intento,puntos,lecturas,arbol`)

---

### Ejecución completa (todos los pasos en secuencia)

```bash
make run-all
```

---

### Limpiar archivos compilados

Elimina los ejecutables y archivos `.o` generados por la compilación (no elimina los árboles ni las métricas):

```bash
make clean
```

---

## Resumen de archivos de salida

| Archivo | Descripción |
|---|---|
| `euConvertido.bin` / `ranConvertido.bin` | Datos convertidos al formato `Pair` |
| `arboles/arbol_*_<exp>.bin` | R-Trees serializados, uno por algoritmo/dataset/experimento |
| `arboles/tiempos.txt` | Tiempos de construcción (CSV) |
| `arboles/metricas.txt` | Resultados de las consultas: puntos encontrados y lecturas de disco (CSV) |

---

## Visualización (opcional)

`graphics/graphics.ipynb` contiene un notebook de Jupyter con los gráficos del experimento. Requiere Python 3 con las siguientes bibliotecas:

```bash
pip install jupyter matplotlib pandas
jupyter notebook graphics/graphics.ipynb
```
