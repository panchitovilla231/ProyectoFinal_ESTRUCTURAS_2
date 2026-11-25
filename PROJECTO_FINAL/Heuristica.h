// Heuristica.h
// Clase abstracta Heuristica y dos clases derivadas.
// POLIMORFISMO: el buscador A* trabaja con un puntero a Heuristica y no
// sabe cual heuristica concreta esta usando.
#ifndef HEURISTICA_H
#define HEURISTICA_H

#include <string>
#include "Tablero.h"

// Clase base abstracta: estima cuantos movimientos faltan para llegar a la meta
class Heuristica {
public:
    virtual ~Heuristica() {}
    // h(n): costo estimado desde 'actual' hasta 'meta'
    virtual int calcular(const Tablero& actual, const Tablero& meta) const = 0;
    virtual std::string obtenerNombre() const = 0;
};

// Distancia de Manhattan: suma de |dfila| + |dcolumna| de cada ficha a su lugar meta
class HeuristicaManhattan : public Heuristica {
public:
    int calcular(const Tablero& actual, const Tablero& meta) const override;
    std::string obtenerNombre() const override;
};

// Piezas mal colocadas (Hamming): cuenta las fichas que no estan en su lugar
class HeuristicaHamming : public Heuristica {
public:
    int calcular(const Tablero& actual, const Tablero& meta) const override;
    std::string obtenerNombre() const override;
};

#endif
