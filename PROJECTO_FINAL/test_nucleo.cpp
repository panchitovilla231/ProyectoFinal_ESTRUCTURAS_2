// test_nucleo.cpp
// Pruebas de consola de la logica (sin interfaz grafica). No es parte del juego:
// sirve para comprobar que A*, los niveles y las puntuaciones funcionan.
#include <cstdio>
#include <iostream>
#include "Juego.h"
#include "Puntuaciones.h"

static int fallos = 0;

// Verifica una condicion e imprime el resultado
static void comprobar(bool condicion, const std::string& descripcion) {
    std::cout << (condicion ? "[OK]    " : "[FALLO] ") << descripcion << std::endl;
    if (!condicion) fallos++;
}

int main() {
    // 1. Manhattan del ejemplo del documento del profesor: h = 4
    std::array<int, 9> ejemplo = { 1, 3, 6, 4, 2, 5, 7, 8, 0 };
    Tablero estadoEjemplo(ejemplo);
    HeuristicaManhattan manhattan;
    comprobar(manhattan.calcular(estadoEjemplo, Tablero()) == 4, "Manhattan del ejemplo del PDF = 4");

    // 2. Paridad: dos casillas intercambiadas => sin solucion
    Tablero imposible(std::array<int, 9>({ 1, 2, 3, 4, 5, 6, 8, 7, 0 }));
    comprobar(!imposible.esResolublePara(Tablero()), "Estado 1 2 3 4 5 6 8 7 _ no tiene solucion");

    // 3. A* resuelve 300 tableros aleatorios y la traza es valida
    BuscadorAEstrella buscador(std::unique_ptr<Heuristica>(new HeuristicaManhattan()));
    bool todosBien = true;
    int maxPasos = 0;
    for (int i = 0; i < 300; i++) {
        Tablero t;
        t.mezclar(200);
        std::vector<Tablero> camino = buscador.resolver(t, Tablero());
        if (camino.empty() || !camino.back().esIgual(Tablero()) || !camino.front().esIgual(t)) { todosBien = false; break; }
        for (size_t k = 1; k < camino.size(); k++) {   // cada paso debe ser UN movimiento legal
            Tablero previo = camino[k - 1];
            if (!previo.moverFicha(camino[k].obtenerPosicionVacia()) || !previo.esIgual(camino[k])) { todosBien = false; }
        }
        if (static_cast<int>(camino.size()) - 1 > maxPasos) maxPasos = static_cast<int>(camino.size()) - 1;
    }
    comprobar(todosBien, "A* resuelve 300 tableros y cada paso es legal (max pasos = " + std::to_string(maxPasos) + ")");

    // 4. A* con meta personalizada
    Tablero metaRara(std::array<int, 9>({ 8, 1, 2, 7, 0, 3, 6, 5, 4 }));
    Tablero inicioRaro = metaRara;
    inicioRaro.mezclar(80);
    SesionInteligente sesion;
    sesion.establecerMetaEstandar();
    sesion.generarMetaAleatoria();
    sesion.generarInicioAleatorio();
    std::string error = sesion.resolver(buscador);
    comprobar(error.empty(), "Modo inteligente resuelve con meta aleatoria");
    sesion.irAlPaso(sesion.obtenerTotalPasos());
    comprobar(sesion.obtenerTableroActual().esIgual(sesion.obtenerMeta()), "La traza termina en la meta");
    std::cout << "        " << sesion.describirPaso(1) << std::endl;

    // 5. Sugerencia: aplicar sugerencias repetidas debe terminar resolviendo el tablero
    Tablero juego;
    juego.mezclar(100);
    int pasos = 0;
    while (!juego.esIgual(Tablero()) && pasos < 200) {
        int pos = buscador.sugerirMovimiento(juego, Tablero(), 30000, 25);
        if (pos < 0 || !juego.moverFicha(pos)) break;
        pasos++;
    }
    comprobar(juego.esIgual(Tablero()), "Siguiendo las sugerencias se resuelve (" + std::to_string(pasos) + " pasos)");

    // 6. Niveles
    PartidaManual partida;
    partida.iniciar(1);
    comprobar(!partida.obtenerTablero().esIgual(Tablero()) && partida.obtenerNivel()->obtenerPistasPermitidas() < 0,
              "Nivel 1: tablero mezclado, pistas ilimitadas");
    partida.iniciar(2);
    comprobar(manhattan.calcular(partida.obtenerTablero(), Tablero()) >= 14 && partida.obtenerNivel()->obtenerLimiteMovimientos() == 100,
              "Nivel 2: mezcla fuerte y limite de 100 movimientos");
    int usadas = 0;
    for (int i = 0; i < 5; i++) if (partida.pedirSugerencia(buscador) >= 0) usadas++;
    comprobar(usadas == 3, "Nivel 2: solo permite 3 pistas");

    // 7. Puntuaciones: acumulan por alias y se ordenan de mayor a menor
    std::remove("prueba_puntos.txt");
    {
        GestorPuntuaciones g("prueba_puntos.txt");
        g.registrarPuntos("ana", 500);
        g.registrarPuntos("beto", 900);
        g.registrarPuntos("ana", 700);   // ana acumula 1200
    }
    GestorPuntuaciones g2("prueba_puntos.txt");  // se vuelve a leer del archivo
    std::vector<RegistroPuntuacion> reporte = g2.obtenerReporteOrdenado();
    comprobar(reporte.size() == 2 && reporte[0].alias == "ana" && reporte[0].puntos == 1200 && reporte[1].alias == "beto",
              "Puntuaciones: acumula por alias, guarda en archivo y ordena");
    std::remove("prueba_puntos.txt");

    std::cout << (fallos == 0 ? "\nTODAS LAS PRUEBAS PASARON" : "\nHAY PRUEBAS FALLIDAS") << std::endl;
    return fallos;
}
