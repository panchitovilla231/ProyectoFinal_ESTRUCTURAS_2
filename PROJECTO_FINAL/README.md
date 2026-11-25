# 8-Puzzle con A* — Estructuras de Datos II (UAA)

Juego del 8-puzzle en C++ con interfaz gráfica (Win32/GDI), dos modos de juego,
dos niveles, sugerencias y solución con el algoritmo A* (heurística de Manhattan),
puntuaciones en archivo de texto y sonidos.

**Integrantes:** _(escribir nombres aquí)_

## Requisitos del sistema
- Windows 10/11
- Visual Studio 2019/2022 con la carga de trabajo "Desarrollo para el escritorio con C++"
- No se necesita instalar ninguna librería externa.

## Cómo compilar y ejecutar (Visual Studio)
1. Archivo → Nuevo → Proyecto → **Proyecto vacío** (C++).
2. Copia todos los archivos de esta carpeta al proyecto y agrégalos con
   *Proyecto → Agregar elemento existente*, **excepto `test_nucleo.cpp`**.
3. Si el proyecto fuera de tipo "Aplicación de consola", cambia
   *Propiedades → Vinculador → Sistema → Subsistema* a **Windows (/SUBSYSTEM:WINDOWS)**.
4. Estándar de C++: C++14 o superior (el predeterminado sirve).
5. Ejecuta con **Ctrl + F5**.

`puntuaciones.txt` se crea solo en la carpeta de trabajo (la del proyecto al ejecutar desde Visual Studio).

### Alternativa con VS Code + MSYS2 (UCRT64 / MinGW-w64)
```
g++ -std=c++14 -O2 Tablero.cpp Heuristica.cpp BuscadorAEstrella.cpp Nivel.cpp Puntuaciones.cpp Juego.cpp Interfaz.cpp -o puzzle8.exe -mwindows
```

### Pruebas de la lógica (opcional)
`test_nucleo.cpp` es un programa de consola que comprueba A*, niveles y puntuaciones:
```
g++ -std=c++14 Tablero.cpp Heuristica.cpp BuscadorAEstrella.cpp Nivel.cpp Puntuaciones.cpp Juego.cpp test_nucleo.cpp -o test_nucleo
```

## Cómo se juega
- **Menú principal:** Modo Manual, Modo Inteligente, Puntuaciones, Reglas, Salir.
- **Modo manual:** escribe tu alias, elige nivel y haz clic en una ficha junto al espacio vacío para moverla.
  *Nueva partida* crea otro tablero aleatorio; *Sugerir movimiento* usa A* y resalta en dorado la ficha a mover.
  Al ganar, los puntos se acumulan por alias en `puntuaciones.txt`.
- **Modo inteligente:** el inicio es aleatorio; puedes editarlo (clic en dos casillas del mismo tablero para
  intercambiarlas) y definir la meta que quieras. *RESOLVER* valida que exista solución y muestra la traza
  paso a paso (Anterior / Siguiente / Reproducir).
- **Puntuaciones:** reporte ordenado de mayor a menor.

## Niveles
| | Nivel 1 – Fácil | Nivel 2 – Difícil |
|---|---|---|
| Mezcla | suave (Manhattan inicial ≥ 6) | fuerte (Manhattan inicial ≥ 14) |
| Pistas (A*) | ilimitadas, −10 pts c/u | 3, −150 pts c/u |
| Límite de movimientos | sin límite | 100 (si se alcanza, se pierde) |
| Puntos | 1000 − 5·mov − 10·pistas − 1·seg (mín. 100) | 3000 − 10·mov − 150·pistas − 2·seg (mín. 200) |

## Organización del código (para el reporte)
| Archivo | Clase(s) | Función |
|---|---|---|
| `Tablero` | `Tablero` | Estado 3x3, movimientos legales, paridad (solubilidad), mezcla |
| `Heuristica` | `Heuristica` (abstracta), `HeuristicaManhattan`, `HeuristicaHamming` | h(n) |
| `BuscadorAEstrella` | `BuscadorAEstrella` | A*: solución completa y sugerencia con árbol limitado |
| `Nivel` | `Nivel` (abstracta), `NivelFacil`, `NivelDificil` | Reglas y puntos de cada nivel |
| `Juego` | `PartidaManual`, `SesionInteligente` | Lógica de cada modo (sin interfaz) |
| `Puntuaciones` | `GestorPuntuaciones` | Archivo de texto `alias;puntos;fecha` |
| `Interfaz` | `Aplicacion` | Ventana, menús, dibujo, clics y sonido |

**POO:** abstracción y encapsulación (atributos privados + métodos de acceso), **herencia** (`Heuristica` y `Nivel`
con dos derivadas cada una), **polimorfismo** (métodos `virtual`/`override` usados mediante punteros a la clase base).

**Estructuras de datos:** `priority_queue` (lista de abiertos de A*), `unordered_map` (mejor g por estado),
`vector` (árbol de búsqueda con índices al padre, traza de la solución), `stack` (reconstrucción del camino),
`list` (registros de puntuaciones), `array` (casillas del tablero).

**Variables globales:** ninguna; todo el estado está en la clase `Aplicacion`.
