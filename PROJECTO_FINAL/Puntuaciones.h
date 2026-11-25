// Puntuaciones.h
// Manejo de archivo de texto con las puntuaciones: alias;puntos;fecha
// Un mismo jugador acumula puntos. Estructura de datos: std::list.
#ifndef PUNTUACIONES_H
#define PUNTUACIONES_H

#include <list>
#include <string>
#include <vector>

// Un renglon del archivo / del reporte
struct RegistroPuntuacion {
    std::string alias;   // nombre del jugador
    int puntos;          // puntos acumulados
    std::string fecha;   // fecha de la ultima partida (AAAA-MM-DD)
};

class GestorPuntuaciones {
public:
    explicit GestorPuntuaciones(const std::string& nombreArchivoPuntuaciones);

    void cargar();                                              // lee el archivo a la lista
    void guardar() const;                                       // escribe la lista al archivo
    void registrarPuntos(const std::string& alias, int puntos); // acumula y guarda
    std::vector<RegistroPuntuacion> obtenerReporteOrdenado() const; // de mayor a menor

private:
    std::string nombreArchivo;
    std::list<RegistroPuntuacion> registros;

    static std::string obtenerFechaActual();
};

#endif
