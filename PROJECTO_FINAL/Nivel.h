// Nivel.h
// Clase abstracta Nivel con dos clases derivadas: NivelFacil y NivelDificil.
// HERENCIA + POLIMORFISMO: la partida usa un puntero a Nivel y cada nivel
// responde distinto (mezcla, pistas, limite de movimientos y puntos).
#ifndef NIVEL_H
#define NIVEL_H

#include <memory>
#include <string>
#include "Tablero.h"

class Nivel {
public:
    virtual ~Nivel() {}

    virtual std::string obtenerNombre() const = 0;
    virtual int obtenerMovimientosMezcla() const = 0;    // pasos aleatorios para mezclar
    virtual int obtenerDistanciaMinima() const = 0;      // Manhattan minimo del tablero inicial
    virtual int obtenerPistasPermitidas() const = 0;     // -1 = ilimitadas
    virtual int obtenerLimiteMovimientos() const = 0;    // 0 = sin limite
    virtual int calcularPuntos(int movimientos, int pistasUsadas, int segundos) const = 0;
    virtual std::string describirReglas() const = 0;     // texto con saltos de linea

    // Metodo comun (plantilla): genera un tablero inicial mezclado segun el nivel
    Tablero generarTableroInicial(const Tablero& meta) const;
};

// Nivel 1: mezcla suave, pistas ilimitadas, sin limite de movimientos
class NivelFacil : public Nivel {
public:
    std::string obtenerNombre() const override;
    int obtenerMovimientosMezcla() const override;
    int obtenerDistanciaMinima() const override;
    int obtenerPistasPermitidas() const override;
    int obtenerLimiteMovimientos() const override;
    int calcularPuntos(int movimientos, int pistasUsadas, int segundos) const override;
    std::string describirReglas() const override;
};

// Nivel 2: mezcla fuerte, 3 pistas y limite de 100 movimientos
class NivelDificil : public Nivel {
public:
    std::string obtenerNombre() const override;
    int obtenerMovimientosMezcla() const override;
    int obtenerDistanciaMinima() const override;
    int obtenerPistasPermitidas() const override;
    int obtenerLimiteMovimientos() const override;
    int calcularPuntos(int movimientos, int pistasUsadas, int segundos) const override;
    std::string describirReglas() const override;
};

// Fabrica: crea el nivel 1 o 2 (devuelve nullptr si el numero no existe)
std::unique_ptr<Nivel> crearNivel(int numeroNivel);

#endif
