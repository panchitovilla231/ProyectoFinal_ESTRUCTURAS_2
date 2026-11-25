// BuscadorAEstrella.h
// Implementa el algoritmo A* para el 8-puzzle.
// Estructuras de datos usadas:
//   - std::priority_queue (cola de prioridad, monton) -> lista de abiertos ordenada por f(n)
//   - std::unordered_map (tabla hash)                 -> mejor g(n) conocido de cada estado
//   - std::vector                                     -> arbol de busqueda (nodos con indice al padre)
//   - std::stack (pila)                               -> reconstruir el camino de la meta a la raiz
#ifndef BUSCADOR_A_ESTRELLA_H
#define BUSCADOR_A_ESTRELLA_H

#include <memory>
#include <vector>
#include "Tablero.h"
#include "Heuristica.h"

class BuscadorAEstrella {
public:
    // El buscador recibe (y es dueno de) la heuristica a utilizar
    explicit BuscadorAEstrella(std::unique_ptr<Heuristica> heuristicaElegida);

    // Busca la solucion completa. Devuelve la lista de tableros desde 'inicio'
    // hasta 'meta' (vacia si no hay solucion o se excede el limite de nodos).
    std::vector<Tablero> resolver(const Tablero& inicio, const Tablero& meta,
                                  int limiteNodos = 400000);

    // Sugiere un movimiento: construye un arbol limitado (altura y nodos maximos)
    // y devuelve la posicion de la ficha que conviene mover (-1 si no hay sugerencia).
    int sugerirMovimiento(const Tablero& actual, const Tablero& meta,
                          int limiteNodos = 30000, int alturaMaxima = 25);

    // Cuantos nodos se expandieron en la ultima busqueda
    int obtenerNodosExpandidos() const;

private:
    // Nodo del arbol de busqueda
    struct Nodo {
        Tablero estado;  // configuracion del tablero
        int g;           // movimientos realizados desde el inicio
        int h;           // heuristica (estimado hasta la meta)
        int padre;       // indice del nodo padre en el vector (-1 = raiz)
    };

    // Elemento de la cola de prioridad
    struct EntradaCola {
        int f;       // g + h
        int h;       // para desempatar
        int indice;  // indice del nodo en el vector
    };

    // Comparador: la cola saca primero el menor f (y en empate el menor h)
    struct ComparadorEntradas {
        bool operator()(const EntradaCola& a, const EntradaCola& b) const {
            if (a.f != b.f) return a.f > b.f;
            return a.h > b.h;
        }
    };

    std::unique_ptr<Heuristica> heuristica;
    int nodosExpandidos;

    // Nucleo de A*. Devuelve el indice del nodo meta si se encontro; si no, el
    // indice del nodo explorado con menor h (el "mas cercano a la solucion").
    int buscar(const Tablero& inicio, const Tablero& meta, int limiteNodos,
               int alturaMaxima, std::vector<Nodo>& nodos, bool& metaEncontrada);

    // Recorre los padres desde un nodo hasta la raiz usando una pila
    std::vector<Tablero> reconstruirCamino(const std::vector<Nodo>& nodos, int indiceFinal) const;
};

#endif
