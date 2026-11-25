// Tablero.cpp
#include "Tablero.h"
#include <cstdlib>
#include <random>

// Constructor por defecto: tablero meta estandar
Tablero::Tablero() {
    for (int i = 0; i < 8; i++) {
        fichas[i] = i + 1;
    }
    fichas[8] = 0;
    posicionVacia = 8;
}

// Constructor con fichas dadas
Tablero::Tablero(const std::array<int, 9>& fichasIniciales) : fichas(fichasIniciales) {
    localizarVacio();
}

// Busca el 0 y guarda su posicion
void Tablero::localizarVacio() {
    posicionVacia = -1;
    for (int i = 0; i < 9; i++) {
        if (fichas[i] == 0) {
            posicionVacia = i;
        }
    }
}

// Devuelve el valor de una casilla (-1 si la posicion es invalida)
int Tablero::obtenerFicha(int posicion) const {
    if (posicion < 0 || posicion > 8) return -1;
    return fichas[posicion];
}

int Tablero::obtenerPosicionVacia() const {
    return posicionVacia;
}

const std::array<int, 9>& Tablero::obtenerFichas() const {
    return fichas;
}

// Una ficha se puede mover si esta a distancia 1 (arriba, abajo, izq. o der.) del vacio
bool Tablero::puedeMoverFicha(int posicion) const {
    if (posicion < 0 || posicion > 8 || posicionVacia < 0) return false;
    if (posicion == posicionVacia) return false;
    int distanciaFilas = std::abs(posicion / 3 - posicionVacia / 3);
    int distanciaColumnas = std::abs(posicion % 3 - posicionVacia % 3);
    return (distanciaFilas + distanciaColumnas) == 1;
}

// Lista las posiciones de las fichas que se pueden deslizar
std::vector<int> Tablero::obtenerFichasMovibles() const {
    std::vector<int> movibles;
    for (int p = 0; p < 9; p++) {
        if (puedeMoverFicha(p)) movibles.push_back(p);
    }
    return movibles;
}

bool Tablero::esIgual(const Tablero& otro) const {
    return fichas == otro.fichas;
}

// Valido = contiene exactamente los numeros 0..8 una sola vez
bool Tablero::esValido() const {
    bool visto[9] = { false };
    for (int i = 0; i < 9; i++) {
        int v = fichas[i];
        if (v < 0 || v > 8 || visto[v]) return false;
        visto[v] = true;
    }
    return true;
}

// Cuenta pares (i<j) con fichas[i] > fichas[j], ignorando el vacio
int Tablero::contarInversiones() const {
    int inversiones = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 9; j++) {
            if (fichas[i] != 0 && fichas[j] != 0 && fichas[i] > fichas[j]) {
                inversiones++;
            }
        }
    }
    return inversiones;
}

// En un tablero de ancho impar (3x3), un estado es alcanzable desde otro
// si y solo si ambos tienen la misma paridad de inversiones.
bool Tablero::esResolublePara(const Tablero& meta) const {
    if (!esValido() || !meta.esValido()) return false;
    return (contarInversiones() % 2) == (meta.contarInversiones() % 2);
}

// Convierte el tablero en un numero de 9 digitos (maximo 876543210)
unsigned int Tablero::calcularClave() const {
    unsigned int clave = 0;
    for (int i = 0; i < 9; i++) {
        clave = clave * 10 + static_cast<unsigned int>(fichas[i]);
    }
    return clave;
}

// Desliza una ficha al espacio vacio si es legal
bool Tablero::moverFicha(int posicion) {
    if (!puedeMoverFicha(posicion)) return false;
    fichas[posicionVacia] = fichas[posicion];
    fichas[posicion] = 0;
    posicionVacia = posicion;
    return true;
}

// Intercambia dos casillas sin respetar reglas (solo para editar inicio/meta)
bool Tablero::intercambiarCasillas(int a, int b) {
    if (a < 0 || a > 8 || b < 0 || b > 8) return false;
    int auxiliar = fichas[a];
    fichas[a] = fichas[b];
    fichas[b] = auxiliar;
    localizarVacio();
    return true;
}

// Mezcla realizando movimientos legales al azar (sin deshacer el anterior).
// Como solo usa movimientos legales, el resultado SIEMPRE tiene solucion.
void Tablero::mezclar(int numeroMovimientos) {
    static std::mt19937 generador{ std::random_device{}() };
    int vaciaPrevia = -1;
    for (int i = 0; i < numeroMovimientos; i++) {
        std::vector<int> candidatas;
        std::vector<int> movibles = obtenerFichasMovibles();
        for (size_t k = 0; k < movibles.size(); k++) {
            if (movibles[k] != vaciaPrevia) candidatas.push_back(movibles[k]);
        }
        if (candidatas.empty()) candidatas = movibles;
        std::uniform_int_distribution<int> sorteo(0, static_cast<int>(candidatas.size()) - 1);
        int elegida = candidatas[sorteo(generador)];
        vaciaPrevia = posicionVacia;
        moverFicha(elegida);
    }
}
