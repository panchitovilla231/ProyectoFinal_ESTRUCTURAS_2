// Tablero.h
// Representa un estado del 8-puzzle: una cuadricula de 3x3 con las fichas 1..8
// y un espacio vacio (representado con el valor 0).
// Posiciones: 0 1 2 / 3 4 5 / 6 7 8  (fila = pos / 3, columna = pos % 3)
#ifndef TABLERO_H
#define TABLERO_H

#include <array>
#include <vector>

class Tablero {
public:
    // Crea el tablero meta estandar: 1 2 3 / 4 5 6 / 7 8 _
    Tablero();
    // Crea un tablero a partir de un arreglo de 9 valores (0 = vacio)
    explicit Tablero(const std::array<int, 9>& fichasIniciales);

    // ---- Consultas ----
    int obtenerFicha(int posicion) const;               // valor en una posicion
    int obtenerPosicionVacia() const;                   // posicion del espacio vacio
    const std::array<int, 9>& obtenerFichas() const;    // acceso de solo lectura
    bool puedeMoverFicha(int posicion) const;           // true si la ficha esta junto al vacio
    std::vector<int> obtenerFichasMovibles() const;     // posiciones de fichas que pueden moverse
    bool esIgual(const Tablero& otro) const;            // compara dos tableros
    bool esValido() const;                              // contiene 0..8 sin repetir
    int contarInversiones() const;                      // inversiones (sin contar el vacio)
    bool esResolublePara(const Tablero& meta) const;    // paridad de inversiones igual
    unsigned int calcularClave() const;                 // numero unico por estado (para tablas hash)

    // ---- Acciones ----
    bool moverFicha(int posicion);                      // desliza la ficha hacia el vacio
    bool intercambiarCasillas(int a, int b);            // intercambia dos casillas (modo edicion)
    void mezclar(int numeroMovimientos);                // mezcla con movimientos legales al azar

private:
    std::array<int, 9> fichas;  // contenido de las 9 casillas
    int posicionVacia;          // posicion del 0

    void localizarVacio();      // recalcula posicionVacia
};

#endif
