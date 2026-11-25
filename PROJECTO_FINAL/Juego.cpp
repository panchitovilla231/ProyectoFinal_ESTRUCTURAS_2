// Juego.cpp
#include "Juego.h"

// ===================== MODO MANUAL =====================

PartidaManual::PartidaManual()
    : movimientos(0), pistasUsadas(0), segundos(0), fichaSugerida(-1), estado(EstadoPartida::JUGANDO) {
}

// Crea una partida nueva: tablero mezclado segun el nivel elegido
void PartidaManual::iniciar(int numeroNivel) {
    nivel = crearNivel(numeroNivel);
    meta = Tablero();
    if (nivel) {
        tablero = nivel->generarTableroInicial(meta);
    } else {
        tablero = meta;
    }
    movimientos = 0;
    pistasUsadas = 0;
    segundos = 0;
    fichaSugerida = -1;
    estado = EstadoPartida::JUGANDO;
}

// Intenta deslizar la ficha de 'posicion'. Despues revisa si se gano o se perdio.
bool PartidaManual::intentarMover(int posicion) {
    if (estado != EstadoPartida::JUGANDO) return false;
    if (!tablero.moverFicha(posicion)) return false;

    movimientos++;
    fichaSugerida = -1;

    if (tablero.esIgual(meta)) {
        estado = EstadoPartida::GANADA;
    } else if (nivel && nivel->obtenerLimiteMovimientos() > 0 &&
               movimientos >= nivel->obtenerLimiteMovimientos()) {
        estado = EstadoPartida::PERDIDA;  // se acabaron los movimientos
    }
    return true;
}

// Indica si todavia quedan pistas disponibles en este nivel
bool PartidaManual::puedePedirPista() const {
    if (!nivel || estado != EstadoPartida::JUGANDO) return false;
    int permitidas = nivel->obtenerPistasPermitidas();
    return permitidas < 0 || pistasUsadas < permitidas;
}

// Sugerencia con A*: se consume una pista solo si se obtuvo una jugada
int PartidaManual::pedirSugerencia(BuscadorAEstrella& buscador) {
    if (!puedePedirPista()) return -1;
    int posicion = buscador.sugerirMovimiento(tablero, meta, 30000, 25);
    if (posicion >= 0) {
        pistasUsadas++;
        fichaSugerida = posicion;
    }
    return posicion;
}

// Solo cuenta el tiempo mientras se esta jugando
void PartidaManual::avanzarSegundo() {
    if (estado == EstadoPartida::JUGANDO && nivel) segundos++;
}

bool PartidaManual::hayPartida() const { return nivel != nullptr; }
const Tablero& PartidaManual::obtenerTablero() const { return tablero; }
const Nivel* PartidaManual::obtenerNivel() const { return nivel.get(); }
int PartidaManual::obtenerMovimientos() const { return movimientos; }
int PartidaManual::obtenerPistasUsadas() const { return pistasUsadas; }
int PartidaManual::obtenerSegundos() const { return segundos; }
int PartidaManual::obtenerFichaSugerida() const { return fichaSugerida; }
EstadoPartida PartidaManual::obtenerEstado() const { return estado; }

int PartidaManual::obtenerPuntos() const {
    if (estado != EstadoPartida::GANADA || !nivel) return 0;
    return nivel->calcularPuntos(movimientos, pistasUsadas, segundos);
}

// ===================== MODO INTELIGENTE =====================

SesionInteligente::SesionInteligente() : pasoActual(0), nodosExpandidos(0) {
    reiniciar();
}

// Meta estandar y un punto de inicio aleatorio
void SesionInteligente::reiniciar() {
    meta = Tablero();
    generarInicioAleatorio();
}

// El inicio se obtiene mezclando la meta actual, asi siempre hay solucion
void SesionInteligente::generarInicioAleatorio() {
    inicio = meta;
    for (int intento = 0; intento < 50 && inicio.esIgual(meta); intento++) {
        inicio.mezclar(60);
    }
    camino.clear();
    pasoActual = 0;
}

void SesionInteligente::establecerMetaEstandar() {
    meta = Tablero();
    camino.clear();
    pasoActual = 0;
}

void SesionInteligente::generarMetaAleatoria() {
    meta = Tablero();
    meta.mezclar(60);
    camino.clear();
    pasoActual = 0;
}

bool SesionInteligente::intercambiarEnInicio(int a, int b) {
    camino.clear();
    pasoActual = 0;
    return inicio.intercambiarCasillas(a, b);
}

bool SesionInteligente::intercambiarEnMeta(int a, int b) {
    camino.clear();
    pasoActual = 0;
    return meta.intercambiarCasillas(a, b);
}

// Valida inicio/meta y ejecuta A* para obtener la traza de la solucion
std::string SesionInteligente::resolver(BuscadorAEstrella& buscador) {
    camino.clear();
    pasoActual = 0;

    if (!inicio.esValido() || !meta.esValido()) {
        return "Los tableros no son validos.";
    }
    if (inicio.esIgual(meta)) {
        return "El inicio y la meta son iguales. Cambia alguno.";
    }
    if (!inicio.esResolublePara(meta)) {
        return "Sin solucion: esa meta es inalcanzable desde el inicio. Intercambia otras casillas.";
    }

    camino = buscador.resolver(inicio, meta);
    nodosExpandidos = buscador.obtenerNodosExpandidos();
    if (camino.empty()) {
        return "No se encontro solucion dentro del limite de busqueda.";
    }
    return "";
}

// Avanza un paso de la traza
bool SesionInteligente::avanzar() {
    if (camino.empty() || pasoActual >= static_cast<int>(camino.size()) - 1) return false;
    pasoActual++;
    return true;
}

// Retrocede un paso de la traza
bool SesionInteligente::retroceder() {
    if (camino.empty() || pasoActual <= 0) return false;
    pasoActual--;
    return true;
}

void SesionInteligente::irAlPaso(int paso) {
    if (camino.empty()) return;
    if (paso < 0) paso = 0;
    if (paso > static_cast<int>(camino.size()) - 1) paso = static_cast<int>(camino.size()) - 1;
    pasoActual = paso;
}

const Tablero& SesionInteligente::obtenerInicio() const { return inicio; }
const Tablero& SesionInteligente::obtenerMeta() const { return meta; }

// Si hay traza se muestra el paso actual; si no, el tablero de inicio
const Tablero& SesionInteligente::obtenerTableroActual() const {
    if (camino.empty()) return inicio;
    return camino[pasoActual];
}

int SesionInteligente::obtenerPasoActual() const { return pasoActual; }

int SesionInteligente::obtenerTotalPasos() const {
    if (camino.empty()) return 0;
    return static_cast<int>(camino.size()) - 1;
}

int SesionInteligente::obtenerNodosExpandidos() const { return nodosExpandidos; }

// Describe el movimiento que lleva del paso anterior al paso indicado
std::string SesionInteligente::describirPaso(int paso) const {
    if (camino.empty()) return "Sin solucion calculada";
    if (paso <= 0) return "Estado inicial";

    const Tablero& anterior = camino[paso - 1];
    const Tablero& actual = camino[paso];
    int posicionFicha = actual.obtenerPosicionVacia();     // donde estaba la ficha movida
    int destino = anterior.obtenerPosicionVacia();         // adonde se deslizo
    int ficha = anterior.obtenerFicha(posicionFicha);

    std::string direccion;
    int diferencia = destino - posicionFicha;
    if (diferencia == -3) direccion = "hacia arriba";
    else if (diferencia == 3) direccion = "hacia abajo";
    else if (diferencia == -1) direccion = "a la izquierda";
    else direccion = "a la derecha";

    return "Paso " + std::to_string(paso) + ": ficha " + std::to_string(ficha) + " " + direccion;
}
