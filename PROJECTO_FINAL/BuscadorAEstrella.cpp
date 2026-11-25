// BuscadorAEstrella.cpp
#include "BuscadorAEstrella.h"
#include <climits>
#include <queue>
#include <stack>
#include <unordered_map>

// Guarda la heuristica que se usara para calcular h(n)
BuscadorAEstrella::BuscadorAEstrella(std::unique_ptr<Heuristica> heuristicaElegida)
    : heuristica(std::move(heuristicaElegida)), nodosExpandidos(0) {
}

int BuscadorAEstrella::obtenerNodosExpandidos() const {
    return nodosExpandidos;
}

// Algoritmo A*: f(n) = g(n) + h(n). Siempre se expande el nodo con menor f.
int BuscadorAEstrella::buscar(const Tablero& inicio, const Tablero& meta, int limiteNodos,
                              int alturaMaxima, std::vector<Nodo>& nodos, bool& metaEncontrada) {
    nodos.clear();
    nodosExpandidos = 0;
    metaEncontrada = false;

    std::priority_queue<EntradaCola, std::vector<EntradaCola>, ComparadorEntradas> abiertos;
    std::unordered_map<unsigned int, int> mejorG;  // clave del estado -> menor g encontrado

    // Nodo raiz (estado inicial)
    int hInicial = heuristica->calcular(inicio, meta);
    Nodo raiz = { inicio, 0, hInicial, -1 };
    nodos.push_back(raiz);
    abiertos.push({ hInicial, hInicial, 0 });
    mejorG[inicio.calcularClave()] = 0;

    // Mejor nodo explorado (distinto de la raiz): menor h y, en empate, menor g
    int mejorIndice = -1;
    int mejorH = INT_MAX;
    int mejorGdelMejor = INT_MAX;

    while (!abiertos.empty()) {
        // 1. Seleccion: se saca el estado con menor f(n)
        EntradaCola entrada = abiertos.top();
        abiertos.pop();
        Nodo actual = nodos[entrada.indice];  // copia (el vector puede crecer)

        // Si ya se encontro un camino mejor a este estado, esta entrada esta vieja
        std::unordered_map<unsigned int, int>::const_iterator it = mejorG.find(actual.estado.calcularClave());
        if (it != mejorG.end() && actual.g > it->second) continue;

        // Prueba de meta
        if (actual.estado.esIgual(meta)) {
            metaEncontrada = true;
            return entrada.indice;
        }

        // Control del tamano del arbol (nodos y altura)
        if (nodosExpandidos >= limiteNodos) break;
        if (actual.g >= alturaMaxima) continue;
        nodosExpandidos++;

        // 2. Expansion: se generan los sucesores (movimientos legales)
        std::vector<int> movibles = actual.estado.obtenerFichasMovibles();
        for (size_t k = 0; k < movibles.size(); k++) {
            Tablero hijo = actual.estado;
            hijo.moverFicha(movibles[k]);
            int gHijo = actual.g + 1;
            unsigned int clave = hijo.calcularClave();

            // Si ya conocemos un camino igual o mejor a este estado, se descarta
            std::unordered_map<unsigned int, int>::const_iterator existente = mejorG.find(clave);
            if (existente != mejorG.end() && existente->second <= gHijo) continue;
            mejorG[clave] = gHijo;

            // 3. Generacion de sucesores: se agregan al arbol y a la cola
            int hHijo = heuristica->calcular(hijo, meta);
            Nodo nuevo = { hijo, gHijo, hHijo, entrada.indice };
            nodos.push_back(nuevo);
            int indiceNuevo = static_cast<int>(nodos.size()) - 1;
            abiertos.push({ gHijo + hHijo, hHijo, indiceNuevo });

            if (hHijo < mejorH || (hHijo == mejorH && gHijo < mejorGdelMejor)) {
                mejorH = hHijo;
                mejorGdelMejor = gHijo;
                mejorIndice = indiceNuevo;
            }
        }
    }

    return (mejorIndice < 0) ? 0 : mejorIndice;
}

// Reconstruye el camino usando una PILA: se apilan los nodos desde la meta
// hasta la raiz y al desapilar quedan en orden inicio -> meta.
std::vector<Tablero> BuscadorAEstrella::reconstruirCamino(const std::vector<Nodo>& nodos, int indiceFinal) const {
    std::stack<int> pila;
    for (int i = indiceFinal; i != -1; i = nodos[i].padre) {
        pila.push(i);
    }
    std::vector<Tablero> camino;
    while (!pila.empty()) {
        camino.push_back(nodos[pila.top()].estado);
        pila.pop();
    }
    return camino;
}

// Solucion completa (optima con Manhattan, que es admisible)
std::vector<Tablero> BuscadorAEstrella::resolver(const Tablero& inicio, const Tablero& meta, int limiteNodos) {
    std::vector<Tablero> camino;
    if (!inicio.esResolublePara(meta)) return camino;  // sin solucion

    std::vector<Nodo> nodos;
    bool encontrada = false;
    int indice = buscar(inicio, meta, limiteNodos, 200, nodos, encontrada);
    if (!encontrada) return camino;
    return reconstruirCamino(nodos, indice);
}

// Sugerencia: arbol de altura/nodos controlados. Se toma el nodo alcanzable mas
// cercano a la meta y se devuelve la primera jugada del camino que lleva a el.
int BuscadorAEstrella::sugerirMovimiento(const Tablero& actual, const Tablero& meta,
                                         int limiteNodos, int alturaMaxima) {
    if (actual.esIgual(meta)) return -1;
    if (!actual.esResolublePara(meta)) return -1;

    std::vector<Nodo> nodos;
    bool encontrada = false;
    int indice = buscar(actual, meta, limiteNodos, alturaMaxima, nodos, encontrada);
    if (indice <= 0) return -1;

    std::vector<Tablero> camino = reconstruirCamino(nodos, indice);
    if (camino.size() < 2) return -1;
    // La ficha que se movio es la que ahora dejo el vacio en su lugar anterior:
    // en el 2do tablero, el vacio esta donde estaba la ficha a mover.
    return camino[1].obtenerPosicionVacia();
}
