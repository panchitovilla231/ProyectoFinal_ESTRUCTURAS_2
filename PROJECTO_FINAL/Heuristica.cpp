// Heuristica.cpp
#include "Heuristica.h"
#include <cstdlib>

// ---------------- Manhattan ----------------
int HeuristicaManhattan::calcular(const Tablero& actual, const Tablero& meta) const {
    // Primero se guarda en que posicion esta cada ficha dentro de la meta
    int posicionEnMeta[9] = { 0 };
    for (int p = 0; p < 9; p++) {
        int valor = meta.obtenerFicha(p);
        if (valor >= 0 && valor <= 8) posicionEnMeta[valor] = p;
    }
    // Se suma la distancia de cada ficha (sin contar el vacio)
    int suma = 0;
    for (int p = 0; p < 9; p++) {
        int valor = actual.obtenerFicha(p);
        if (valor > 0) {
            int destino = posicionEnMeta[valor];
            suma += std::abs(p / 3 - destino / 3) + std::abs(p % 3 - destino % 3);
        }
    }
    return suma;
}

std::string HeuristicaManhattan::obtenerNombre() const {
    return "Distancia de Manhattan";
}

// ---------------- Hamming ----------------
int HeuristicaHamming::calcular(const Tablero& actual, const Tablero& meta) const {
    int malColocadas = 0;
    for (int p = 0; p < 9; p++) {
        int valor = actual.obtenerFicha(p);
        if (valor != 0 && valor != meta.obtenerFicha(p)) malColocadas++;
    }
    return malColocadas;
}

std::string HeuristicaHamming::obtenerNombre() const {
    return "Piezas mal colocadas (Hamming)";
}
