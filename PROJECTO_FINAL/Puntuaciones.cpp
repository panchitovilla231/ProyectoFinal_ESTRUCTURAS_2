// Puntuaciones.cpp
#include "Puntuaciones.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <string>

GestorPuntuaciones::GestorPuntuaciones(const std::string& nombreArchivoPuntuaciones)
    : nombreArchivo(nombreArchivoPuntuaciones) {
    cargar();
}

// Fecha actual con formato AAAA-MM-DD
std::string GestorPuntuaciones::obtenerFechaActual() {
    std::time_t ahora = std::time(nullptr);
    std::tm info = {};
#ifdef _WIN32
    localtime_s(&info, &ahora);
#else
    localtime_r(&ahora, &info);
#endif
    char texto[16];
    std::strftime(texto, sizeof(texto), "%Y-%m-%d", &info);
    return std::string(texto);
}

// Lee el archivo de texto. Cada linea: alias;puntos;fecha
void GestorPuntuaciones::cargar() {
    registros.clear();
    std::ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) return;  // primera vez: aun no existe

    std::string linea;
    while (std::getline(archivo, linea)) {
        size_t p1 = linea.find(';');
        size_t p2 = (p1 == std::string::npos) ? std::string::npos : linea.find(';', p1 + 1);
        if (p1 == std::string::npos || p2 == std::string::npos) continue;  // linea mal formada

        RegistroPuntuacion registro;
        registro.alias = linea.substr(0, p1);
        registro.fecha = linea.substr(p2 + 1);
        try {
            registro.puntos = std::stoi(linea.substr(p1 + 1, p2 - p1 - 1));
        } catch (...) {
            continue;  // puntos no numericos: se ignora la linea
        }
        registros.push_back(registro);
    }
}

// Reescribe el archivo completo con los registros actuales
void GestorPuntuaciones::guardar() const {
    std::ofstream archivo(nombreArchivo.c_str(), std::ios::trunc);
    if (!archivo.is_open()) return;
    for (std::list<RegistroPuntuacion>::const_iterator it = registros.begin(); it != registros.end(); ++it) {
        archivo << it->alias << ';' << it->puntos << ';' << it->fecha << '\n';
    }
}

// Si el alias ya existe se le suman los puntos; si no, se crea un registro nuevo
void GestorPuntuaciones::registrarPuntos(const std::string& alias, int puntos) {
    std::string fecha = obtenerFechaActual();
    for (std::list<RegistroPuntuacion>::iterator it = registros.begin(); it != registros.end(); ++it) {
        if (it->alias == alias) {
            it->puntos += puntos;
            it->fecha = fecha;
            guardar();
            return;
        }
    }
    RegistroPuntuacion nuevo;
    nuevo.alias = alias;
    nuevo.puntos = puntos;
    nuevo.fecha = fecha;
    registros.push_back(nuevo);
    guardar();
}

// Comparador para ordenar de mayor a menor puntaje (empate: alfabetico)
static bool mayorPuntaje(const RegistroPuntuacion& a, const RegistroPuntuacion& b) {
    if (a.puntos != b.puntos) return a.puntos > b.puntos;
    return a.alias < b.alias;
}

std::vector<RegistroPuntuacion> GestorPuntuaciones::obtenerReporteOrdenado() const {
    std::vector<RegistroPuntuacion> reporte(registros.begin(), registros.end());
    std::sort(reporte.begin(), reporte.end(), mayorPuntaje);
    return reporte;
}
