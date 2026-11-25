// Nivel.cpp
#include "Nivel.h"
#include "Heuristica.h"
#include <algorithm>

// Genera un tablero inicial resoluble: mezcla la meta con movimientos legales
// y repite hasta que el tablero este lo bastante desordenado para el nivel.
Tablero Nivel::generarTableroInicial(const Tablero& meta) const {
    HeuristicaManhattan manhattan;
    Tablero tablero = meta;
    for (int intento = 0; intento < 500; intento++) {
        tablero = meta;
        tablero.mezclar(obtenerMovimientosMezcla());
        if (!tablero.esIgual(meta) && manhattan.calcular(tablero, meta) >= obtenerDistanciaMinima()) {
            break;
        }
    }
    return tablero;
}

// ---------------- Nivel 1: Facil ----------------
std::string NivelFacil::obtenerNombre() const { return "Nivel 1 - Facil"; }
int NivelFacil::obtenerMovimientosMezcla() const { return 40; }
int NivelFacil::obtenerDistanciaMinima() const { return 6; }
int NivelFacil::obtenerPistasPermitidas() const { return -1; }
int NivelFacil::obtenerLimiteMovimientos() const { return 0; }

// Puntos = 1000 - 5 por movimiento - 10 por pista - 1 por segundo (minimo 100)
int NivelFacil::calcularPuntos(int movimientos, int pistasUsadas, int segundos) const {
    return std::max(100, 1000 - 5 * movimientos - 10 * pistasUsadas - segundos);
}

std::string NivelFacil::describirReglas() const {
    return "Mezcla suave del tablero.\n"
           "Pistas ilimitadas (cada una resta 10 puntos).\n"
           "Sin limite de movimientos.\n"
           "Puntos = 1000 - 5 por movimiento - 10 por pista - 1 por segundo (minimo 100).";
}

// ---------------- Nivel 2: Dificil ----------------
std::string NivelDificil::obtenerNombre() const { return "Nivel 2 - Dificil"; }
int NivelDificil::obtenerMovimientosMezcla() const { return 200; }
int NivelDificil::obtenerDistanciaMinima() const { return 14; }
int NivelDificil::obtenerPistasPermitidas() const { return 3; }
int NivelDificil::obtenerLimiteMovimientos() const { return 100; }

// Puntos = 3000 - 10 por movimiento - 150 por pista - 2 por segundo (minimo 200)
int NivelDificil::calcularPuntos(int movimientos, int pistasUsadas, int segundos) const {
    return std::max(200, 3000 - 10 * movimientos - 150 * pistasUsadas - 2 * segundos);
}

std::string NivelDificil::describirReglas() const {
    return "Mezcla fuerte del tablero.\n"
           "Solo 3 pistas (cada una resta 150 puntos).\n"
           "Limite de 100 movimientos: si lo alcanzas sin resolver, pierdes.\n"
           "Puntos = 3000 - 10 por movimiento - 150 por pista - 2 por segundo (minimo 200).";
}

// ---------------- Fabrica ----------------
std::unique_ptr<Nivel> crearNivel(int numeroNivel) {
    if (numeroNivel == 1) return std::unique_ptr<Nivel>(new NivelFacil());
    if (numeroNivel == 2) return std::unique_ptr<Nivel>(new NivelDificil());
    return std::unique_ptr<Nivel>();
}
