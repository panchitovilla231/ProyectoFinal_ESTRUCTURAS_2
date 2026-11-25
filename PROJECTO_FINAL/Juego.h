// Juego.h
// Logica de los dos modos de juego, sin nada de interfaz grafica:
//   - PartidaManual: el jugador desplaza las piezas.
//   - SesionInteligente: el usuario define inicio y meta y A* resuelve paso a paso.
#ifndef JUEGO_H
#define JUEGO_H

#include <memory>
#include <string>
#include <vector>
#include "Tablero.h"
#include "Nivel.h"
#include "BuscadorAEstrella.h"

enum class EstadoPartida { JUGANDO, GANADA, PERDIDA };

// ===================== MODO MANUAL =====================
class PartidaManual {
public:
    PartidaManual();

    void iniciar(int numeroNivel);              // nueva partida aleatoria del nivel dado
    bool intentarMover(int posicion);           // mueve la ficha si es legal
    int pedirSugerencia(BuscadorAEstrella& buscador); // A*: devuelve posicion a mover o -1
    void avanzarSegundo();                      // el reloj de la interfaz llama esto cada segundo

    bool puedePedirPista() const;
    bool hayPartida() const;
    const Tablero& obtenerTablero() const;
    const Nivel* obtenerNivel() const;
    int obtenerMovimientos() const;
    int obtenerPistasUsadas() const;
    int obtenerSegundos() const;
    int obtenerFichaSugerida() const;           // posicion resaltada (-1 = ninguna)
    EstadoPartida obtenerEstado() const;
    int obtenerPuntos() const;                  // puntos ganados (0 si no se gano)

private:
    Tablero tablero;
    Tablero meta;
    std::unique_ptr<Nivel> nivel;
    int movimientos;
    int pistasUsadas;
    int segundos;
    int fichaSugerida;
    EstadoPartida estado;
};

// ===================== MODO INTELIGENTE =====================
class SesionInteligente {
public:
    SesionInteligente();

    void reiniciar();                           // meta estandar + inicio aleatorio
    void generarInicioAleatorio();              // inicio = meta mezclada (siempre resoluble)
    void establecerMetaEstandar();
    void generarMetaAleatoria();
    bool intercambiarEnInicio(int a, int b);    // edicion manual del inicio
    bool intercambiarEnMeta(int a, int b);      // edicion manual de la meta

    // Ejecuta A*. Devuelve "" si todo salio bien o el mensaje de error.
    std::string resolver(BuscadorAEstrella& buscador);

    // Reproduccion paso a paso de la traza de soluciones
    bool avanzar();
    bool retroceder();
    void irAlPaso(int paso);

    const Tablero& obtenerInicio() const;
    const Tablero& obtenerMeta() const;
    const Tablero& obtenerTableroActual() const;
    int obtenerPasoActual() const;
    int obtenerTotalPasos() const;              // movimientos de la solucion
    int obtenerNodosExpandidos() const;
    std::string describirPaso(int paso) const;  // "Paso 3: ficha 5 hacia arriba"

private:
    Tablero inicio;
    Tablero meta;
    std::vector<Tablero> camino;                // traza: inicio ... meta
    int pasoActual;
    int nodosExpandidos;
};

#endif
