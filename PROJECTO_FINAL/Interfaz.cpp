// Interfaz.cpp
// Interfaz grafica del 8-puzzle con la libreria raylib (Windows, macOS y Linux).
// Los sonidos se generan por codigo (ondas senoidales), sin archivos externos.
//
// raylib trabaja en "modo inmediato": en cada cuadro (60 por segundo) se leen
// las entradas (actualizar) y se dibuja toda la pantalla (dibujar).
// Todo el estado vive dentro de la clase Aplicacion (no hay variables globales).
#include "raylib.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "BuscadorAEstrella.h"
#include "Heuristica.h"
#include "Juego.h"
#include "Nivel.h"
#include "Puntuaciones.h"
#include "Tablero.h"

class Aplicacion {
public:
    Aplicacion();
    int ejecutar();  // crea la ventana y corre el ciclo principal

private:
    // Colores de la interfaz (constantes de clase)
    static constexpr Color C_FONDO = { 24, 28, 44, 255 };
    static constexpr Color C_PANEL = { 36, 42, 66, 255 };
    static constexpr Color C_BORDE = { 60, 70, 105, 255 };
    static constexpr Color C_FICHA = { 236, 240, 250, 255 };
    static constexpr Color C_TEXTO_FICHA = { 30, 36, 60, 255 };
    static constexpr Color C_HUECO = { 20, 24, 38, 255 };
    static constexpr Color C_DORADO = { 255, 196, 61, 255 };
    static constexpr Color C_AZUL = { 66, 133, 244, 255 };
    static constexpr Color C_VERDE = { 70, 190, 120, 255 };
    static constexpr Color C_ROJO = { 230, 90, 90, 255 };
    static constexpr Color C_TEXTO = { 230, 235, 250, 255 };
    static constexpr Color C_TEXTO_SUAVE = { 150, 160, 190, 255 };
    static constexpr Color C_BOTON = { 58, 92, 200, 255 };
    static constexpr Color C_BOTON_HOVER = { 88, 122, 230, 255 };
    static constexpr Color C_BOTON_OFF = { 70, 75, 95, 255 };
    static constexpr Color C_BLANCO = { 255, 255, 255, 255 };

    // Pantallas del programa (cada una es una "opcion de menu")
    enum class Pantalla { MENU, ALIAS, ELEGIR_NIVEL, MANUAL, CONFIG_INTELIGENTE, REPRODUCCION, PUNTAJES, REGLAS };

    // Acciones que puede disparar un boton
    enum Accion {
        A_MODO_MANUAL, A_MODO_INTELIGENTE, A_PUNTAJES, A_REGLAS, A_SALIR, A_MENU,
        A_CONTINUAR_ALIAS, A_NIVEL_1, A_NIVEL_2,
        A_NUEVA_PARTIDA, A_SUGERIR,
        A_INICIO_ALEATORIO, A_META_ESTANDAR, A_META_ALEATORIA, A_RESOLVER,
        A_ANTERIOR, A_SIGUIENTE, A_AUTO, A_REINICIAR_TRAZA, A_VOLVER_CONFIG
    };

    struct Boton {
        Rectangle area;
        std::string texto;
        Accion accion;
        bool habilitado;
    };

    // ---- Datos ----
    Pantalla pantalla;
    std::vector<Boton> botones;
    std::string alias;
    std::string mensaje;
    int numeroNivelActual;
    bool puntosGuardados;
    bool reproduciendo;
    bool salir;
    int seleccionTablero;      // 0 = ninguna, 1 = inicio, 2 = meta (modo inteligente)
    int seleccionPosicion;
    float tiempoReloj;         // segundos acumulados para el reloj de la partida
    float tiempoAuto;          // segundos acumulados para la reproduccion automatica

    bool audioListo;
    Sound sonidoMover, sonidoFallo, sonidoAyuda, sonidoGanar;

    PartidaManual partida;
    SesionInteligente sesion;
    BuscadorAEstrella buscador;
    GestorPuntuaciones gestor;
    std::vector<RegistroPuntuacion> reporte;

    // ---- Ciclo principal ----
    void actualizar();
    void dibujar();

    // ---- Dibujo de pantallas ----
    void construirBotones();
    void dibujarBotones();
    void dibujarMenu();
    void dibujarAlias();
    void dibujarElegirNivel(bool conBotones);
    void dibujarManual();
    void dibujarConfigInteligente();
    void dibujarReproduccion();
    void dibujarPuntajes();
    void dibujarTablero(const Tablero& tablero, int x0, int y0, int tam, int tamanoFuente,
                        int resaltada, int seleccionada);
    void dibujarTarjetaNivel(int numeroNivel, Rectangle area);

    // ---- Primitivas de dibujo ----
    void rectRedondeado(Rectangle r, Color relleno, Color borde, float radio);
    void textoIzquierda(const std::string& texto, int x, int y, int tamano, Color color);
    void textoIzquierdaAjustado(const std::string& texto, int x, int y, int anchoMaximo, int tamano, Color color);
    void textoCentrado(const std::string& texto, Rectangle area, int tamano, Color color, bool centrarVertical);
    void textoAjustado(const std::string& texto, Rectangle area, int tamano, Color color);
    void agregarBoton(float x, float y, float ancho, float alto, Accion accion, const std::string& texto,
                      bool habilitado = true);

    // ---- Geometria del tablero ----
    static Rectangle rectCelda(int x0, int y0, int tam, int posicion);
    static int posicionDeClic(int x0, int y0, int tam, int px, int py);

    // ---- Eventos y acciones ----
    void alHacerClic(int x, int y);
    void leerTecladoAlias();
    void ejecutarAccion(Accion accion);
    void clicEnManual(int x, int y);
    void clicEnConfig(int x, int y);
    void revisarFinDePartida();

    // ---- Utilidades y sonido ----
    static std::string formatearTiempo(int segundos);
    static std::vector<std::string> separar(const std::string& texto, char separador);
    static int tamanoParaAncho(const std::string& texto, int anchoMaximo, int tamano);
    Sound crearSonido(const std::vector<std::pair<int, int>>& tonos);
    void iniciarAudio();
    void cerrarAudio();
    void reproducir(const Sound& sonido);
    void sonidoMovimiento();
    void sonidoError();
    void sonidoPista();
    void sonidoVictoria();
};

// ======================================================================
//  Construccion y ciclo principal
// ======================================================================

Aplicacion::Aplicacion()
    : pantalla(Pantalla::MENU), numeroNivelActual(1), puntosGuardados(false), reproduciendo(false),
      salir(false), seleccionTablero(0), seleccionPosicion(-1), tiempoReloj(0.0f), tiempoAuto(0.0f),
      audioListo(false), sonidoMover(), sonidoFallo(), sonidoAyuda(), sonidoGanar(),
      buscador(std::unique_ptr<Heuristica>(new HeuristicaManhattan())),
      gestor("puntuaciones.txt") {
}

// Crea la ventana, el audio y corre el ciclo hasta que se cierre
int Aplicacion::ejecutar() {
    InitWindow(760, 590, "8-Puzzle - Estructuras de Datos II");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);  // ESC no cierra la ventana (se usa para volver al menu)
    SetTextureFilter(GetFontDefault().texture, TEXTURE_FILTER_BILINEAR);  // texto mas suave
    iniciarAudio();

    while (!salir && !WindowShouldClose()) {
        actualizar();
        dibujar();
    }

    cerrarAudio();
    CloseWindow();
    return 0;
}

// Lee entradas y avanza los relojes (se llama una vez por cuadro)
void Aplicacion::actualizar() {
    float dt = GetFrameTime();
    construirBotones();

    // Reloj de la partida manual: cuenta segundos solo mientras se juega
    tiempoReloj += dt;
    while (tiempoReloj >= 1.0f) {
        tiempoReloj -= 1.0f;
        if (pantalla == Pantalla::MANUAL) partida.avanzarSegundo();
    }

    // Reproduccion automatica de la solucion (un paso cada 0.7 s)
    if (pantalla == Pantalla::REPRODUCCION && reproduciendo) {
        tiempoAuto += dt;
        if (tiempoAuto >= 0.7f) {
            tiempoAuto = 0.0f;
            if (sesion.avanzar()) {
                sonidoMovimiento();
            } else {
                reproduciendo = false;  // llego a la meta
                sonidoVictoria();
            }
        }
    }

    // Teclado
    if (pantalla == Pantalla::ALIAS) leerTecladoAlias();
    if (IsKeyPressed(KEY_ESCAPE) && pantalla != Pantalla::MENU) ejecutarAccion(A_MENU);

    // Raton
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        alHacerClic(GetMouseX(), GetMouseY());
    }
}

// Captura de teclado para escribir el alias
void Aplicacion::leerTecladoAlias() {
    int caracter = GetCharPressed();
    while (caracter > 0) {
        if (alias.size() < 12 && caracter < 128 && (std::isalnum(caracter) || caracter == '_')) {
            alias.push_back(static_cast<char>(caracter));
        }
        caracter = GetCharPressed();
    }
    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !alias.empty()) {
        alias.erase(alias.size() - 1);
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        ejecutarAccion(A_CONTINUAR_ALIAS);
    }
}

// ======================================================================
//  Clics del raton
// ======================================================================

void Aplicacion::alHacerClic(int x, int y) {
    // 1. Primero se revisan los botones de la pantalla actual
    for (size_t i = 0; i < botones.size(); i++) {
        const Boton& b = botones[i];
        if (x >= b.area.x && x < b.area.x + b.area.width && y >= b.area.y && y < b.area.y + b.area.height) {
            if (b.habilitado) {
                ejecutarAccion(b.accion);
            } else {
                sonidoError();
            }
            return;
        }
    }
    // 2. Si no fue un boton, puede ser un clic sobre el tablero
    if (pantalla == Pantalla::MANUAL) clicEnManual(x, y);
    else if (pantalla == Pantalla::CONFIG_INTELIGENTE) clicEnConfig(x, y);
}

// Modo manual: clic en una ficha junto al espacio vacio -> se mueve
void Aplicacion::clicEnManual(int x, int y) {
    if (!partida.hayPartida() || partida.obtenerEstado() != EstadoPartida::JUGANDO) return;
    int posicion = posicionDeClic(30, 80, 440, x, y);
    if (posicion < 0) return;
    if (partida.intentarMover(posicion)) {
        mensaje.clear();
        sonidoMovimiento();
        revisarFinDePartida();
    } else {
        mensaje = "Esa ficha no esta junto al espacio vacio.";
        sonidoError();
    }
}

// Modo inteligente: clic en dos casillas del mismo tablero las intercambia
void Aplicacion::clicEnConfig(int x, int y) {
    int tablero = 1;
    int posicion = posicionDeClic(30, 115, 320, x, y);
    if (posicion < 0) {
        tablero = 2;
        posicion = posicionDeClic(400, 115, 320, x, y);
    }
    if (posicion < 0) return;

    if (seleccionTablero == tablero && seleccionPosicion == posicion) {
        seleccionTablero = 0;                      // segundo clic en la misma casilla: se deselecciona
    } else if (seleccionTablero == tablero) {
        if (tablero == 1) sesion.intercambiarEnInicio(seleccionPosicion, posicion);
        else sesion.intercambiarEnMeta(seleccionPosicion, posicion);
        seleccionTablero = 0;
        mensaje = "Casillas intercambiadas.";
        sonidoMovimiento();
    } else {
        seleccionTablero = tablero;                // primer clic: se selecciona
        seleccionPosicion = posicion;
        mensaje.clear();
    }
}

// Revisa si la partida manual termino y, si se gano, guarda los puntos en el archivo
void Aplicacion::revisarFinDePartida() {
    EstadoPartida estado = partida.obtenerEstado();
    if (estado == EstadoPartida::GANADA && !puntosGuardados) {
        int puntos = partida.obtenerPuntos();
        gestor.registrarPuntos(alias, puntos);
        puntosGuardados = true;
        mensaje = "GANASTE! +" + std::to_string(puntos) + " puntos para " + alias + " (guardados).";
        sonidoVictoria();
    } else if (estado == EstadoPartida::PERDIDA) {
        mensaje = "Se acabaron los movimientos. Perdiste esta partida.";
        sonidoError();
    }
}

// ======================================================================
//  Acciones de los botones (la logica de los menus)
// ======================================================================

void Aplicacion::ejecutarAccion(Accion accion) {
    switch (accion) {
    case A_MODO_MANUAL:
        pantalla = Pantalla::ALIAS;
        mensaje.clear();
        break;
    case A_CONTINUAR_ALIAS:
        if (alias.empty()) {
            mensaje = "Escribe un alias para continuar.";
            sonidoError();
        } else {
            pantalla = Pantalla::ELEGIR_NIVEL;
            mensaje.clear();
        }
        break;
    case A_NIVEL_1:
    case A_NIVEL_2:
        numeroNivelActual = (accion == A_NIVEL_1) ? 1 : 2;
        partida.iniciar(numeroNivelActual);
        puntosGuardados = false;
        tiempoReloj = 0.0f;
        mensaje.clear();
        pantalla = Pantalla::MANUAL;
        break;
    case A_NUEVA_PARTIDA:
        partida.iniciar(numeroNivelActual);
        puntosGuardados = false;
        tiempoReloj = 0.0f;
        mensaje = "Nueva partida creada.";
        break;
    case A_SUGERIR: {
        int posicion = partida.pedirSugerencia(buscador);
        if (posicion >= 0) {
            mensaje = "Sugerencia (A*): mueve la ficha " +
                      std::to_string(partida.obtenerTablero().obtenerFicha(posicion)) + " (resaltada).";
            sonidoPista();
        } else {
            mensaje = "No hay sugerencia disponible.";
            sonidoError();
        }
        break;
    }
    case A_MODO_INTELIGENTE:
        sesion.reiniciar();
        seleccionTablero = 0;
        mensaje = "Edita inicio y meta (o usa los botones) y pulsa RESOLVER.";
        pantalla = Pantalla::CONFIG_INTELIGENTE;
        break;
    case A_INICIO_ALEATORIO:
        sesion.generarInicioAleatorio();
        seleccionTablero = 0;
        mensaje = "Nuevo inicio aleatorio.";
        break;
    case A_META_ESTANDAR:
        sesion.establecerMetaEstandar();
        seleccionTablero = 0;
        mensaje = "Meta estandar: 1 2 3 / 4 5 6 / 7 8 _";
        break;
    case A_META_ALEATORIA:
        sesion.generarMetaAleatoria();
        seleccionTablero = 0;
        mensaje = "Meta aleatoria. Si el inicio no llega a ella, RESOLVER lo avisara.";
        break;
    case A_RESOLVER: {
        std::string error = sesion.resolver(buscador);
        if (error.empty()) {
            reproduciendo = false;
            mensaje.clear();
            pantalla = Pantalla::REPRODUCCION;
            sonidoPista();
        } else {
            mensaje = error;
            sonidoError();
        }
        break;
    }
    case A_ANTERIOR:
        if (sesion.retroceder()) sonidoMovimiento();
        break;
    case A_SIGUIENTE:
        if (sesion.avanzar()) sonidoMovimiento();
        break;
    case A_AUTO:
        reproduciendo = !reproduciendo;
        tiempoAuto = 0.0f;
        if (reproduciendo && sesion.obtenerPasoActual() >= sesion.obtenerTotalPasos()) sesion.irAlPaso(0);
        break;
    case A_REINICIAR_TRAZA:
        sesion.irAlPaso(0);
        break;
    case A_VOLVER_CONFIG:
        reproduciendo = false;
        seleccionTablero = 0;
        mensaje.clear();
        pantalla = Pantalla::CONFIG_INTELIGENTE;
        break;
    case A_PUNTAJES:
        gestor.cargar();
        reporte = gestor.obtenerReporteOrdenado();
        pantalla = Pantalla::PUNTAJES;
        break;
    case A_REGLAS:
        pantalla = Pantalla::REGLAS;
        break;
    case A_MENU:
        reproduciendo = false;
        mensaje.clear();
        pantalla = Pantalla::MENU;
        break;
    case A_SALIR:
        salir = true;
        break;
    }
}

// ======================================================================
//  Botones de cada pantalla
// ======================================================================

void Aplicacion::agregarBoton(float x, float y, float ancho, float alto, Accion accion,
                              const std::string& texto, bool habilitado) {
    Boton b;
    b.area = Rectangle{ x, y, ancho, alto };
    b.texto = texto;
    b.accion = accion;
    b.habilitado = habilitado;
    botones.push_back(b);
}

// Reconstruye la lista de botones segun la pantalla actual
void Aplicacion::construirBotones() {
    botones.clear();
    switch (pantalla) {
    case Pantalla::MENU:
        agregarBoton(230, 150, 300, 48, A_MODO_MANUAL, "Modo Manual");
        agregarBoton(230, 212, 300, 48, A_MODO_INTELIGENTE, "Modo Inteligente");
        agregarBoton(230, 274, 300, 48, A_PUNTAJES, "Puntuaciones");
        agregarBoton(230, 336, 300, 48, A_REGLAS, "Reglas de los niveles");
        agregarBoton(230, 398, 300, 48, A_SALIR, "EXIT");
        break;
    case Pantalla::ALIAS:
        agregarBoton(230, 320, 300, 48, A_CONTINUAR_ALIAS, "Continuar");
        agregarBoton(230, 382, 300, 48, A_MENU, "Volver al menu");
        break;
    case Pantalla::ELEGIR_NIVEL:
        agregarBoton(60, 398, 280, 48, A_NIVEL_1, "Jugar Nivel 1");
        agregarBoton(420, 398, 280, 48, A_NIVEL_2, "Jugar Nivel 2");
        agregarBoton(230, 470, 300, 48, A_MENU, "Volver al menu");
        break;
    case Pantalla::REGLAS:
        agregarBoton(230, 470, 300, 48, A_MENU, "Volver al menu");
        break;
    case Pantalla::MANUAL:
        agregarBoton(500, 350, 240, 44, A_NUEVA_PARTIDA, "Nueva partida");
        agregarBoton(500, 402, 240, 44, A_SUGERIR, "Sugerir movimiento", partida.puedePedirPista());
        agregarBoton(500, 454, 240, 44, A_MENU, "Menu principal");
        break;
    case Pantalla::CONFIG_INTELIGENTE:
        agregarBoton(30, 478, 170, 44, A_INICIO_ALEATORIO, "Inicio aleatorio");
        agregarBoton(210, 478, 170, 44, A_META_ESTANDAR, "Meta estandar");
        agregarBoton(390, 478, 170, 44, A_META_ALEATORIA, "Meta aleatoria");
        agregarBoton(570, 478, 160, 44, A_RESOLVER, "RESOLVER");
        agregarBoton(30, 530, 170, 40, A_MENU, "Menu principal");
        break;
    case Pantalla::REPRODUCCION:
        agregarBoton(500, 300, 115, 44, A_ANTERIOR, "< Anterior", sesion.obtenerPasoActual() > 0);
        agregarBoton(625, 300, 115, 44, A_SIGUIENTE, "Siguiente >",
                     sesion.obtenerPasoActual() < sesion.obtenerTotalPasos());
        agregarBoton(500, 352, 240, 44, A_AUTO, reproduciendo ? "Pausar" : "Reproducir solucion");
        agregarBoton(500, 404, 240, 44, A_REINICIAR_TRAZA, "Volver al paso 0");
        agregarBoton(500, 456, 240, 44, A_VOLVER_CONFIG, "Cambiar inicio/meta");
        agregarBoton(500, 508, 240, 44, A_MENU, "Menu principal");
        break;
    case Pantalla::PUNTAJES:
        agregarBoton(230, 520, 300, 44, A_MENU, "Volver al menu");
        break;
    }
}

// ======================================================================
//  Dibujo
// ======================================================================

// Dibuja un cuadro completo
void Aplicacion::dibujar() {
    BeginDrawing();
    ClearBackground(C_FONDO);

    construirBotones();
    switch (pantalla) {
    case Pantalla::MENU: dibujarMenu(); break;
    case Pantalla::ALIAS: dibujarAlias(); break;
    case Pantalla::ELEGIR_NIVEL: dibujarElegirNivel(true); break;
    case Pantalla::REGLAS: dibujarElegirNivel(false); break;
    case Pantalla::MANUAL: dibujarManual(); break;
    case Pantalla::CONFIG_INTELIGENTE: dibujarConfigInteligente(); break;
    case Pantalla::REPRODUCCION: dibujarReproduccion(); break;
    case Pantalla::PUNTAJES: dibujarPuntajes(); break;
    }
    dibujarBotones();

    EndDrawing();
}

// Rectangulo con esquinas redondeadas y borde (dos capas: borde y relleno)
void Aplicacion::rectRedondeado(Rectangle r, Color relleno, Color borde, float radio) {
    float ladoMenor = std::min(r.width, r.height);
    float redondez = std::min(1.0f, 2.0f * radio / ladoMenor);
    DrawRectangleRounded(r, redondez, 10, borde);
    Rectangle interior = { r.x + 2, r.y + 2, r.width - 4, r.height - 4 };
    float redondezInterior = std::min(1.0f, 2.0f * std::max(0.0f, radio - 2) / std::min(interior.width, interior.height));
    DrawRectangleRounded(interior, redondezInterior, 10, relleno);
}

void Aplicacion::textoIzquierda(const std::string& texto, int x, int y, int tamano, Color color) {
    DrawText(texto.c_str(), x, y, tamano, color);
}

// Reduce el tamano de letra si el texto no cabe en 'anchoMaximo'
int Aplicacion::tamanoParaAncho(const std::string& texto, int anchoMaximo, int tamano) {
    while (tamano > 10 && MeasureText(texto.c_str(), tamano) > anchoMaximo) tamano--;
    return tamano;
}

void Aplicacion::textoIzquierdaAjustado(const std::string& texto, int x, int y, int anchoMaximo, int tamano, Color color) {
    DrawText(texto.c_str(), x, y, tamanoParaAncho(texto, anchoMaximo, tamano), color);
}

// Texto de una linea centrado horizontalmente (y opcionalmente vertical) dentro de 'area'
void Aplicacion::textoCentrado(const std::string& texto, Rectangle area, int tamano, Color color, bool centrarVertical) {
    int ancho = MeasureText(texto.c_str(), tamano);
    float x = area.x + (area.width - ancho) / 2.0f;
    float y = centrarVertical ? area.y + (area.height - tamano) / 2.0f : area.y;
    DrawText(texto.c_str(), static_cast<int>(x), static_cast<int>(y), tamano, color);
}

// Divide un texto por un caracter separador
std::vector<std::string> Aplicacion::separar(const std::string& texto, char separador) {
    std::vector<std::string> partes;
    std::string actual;
    for (size_t i = 0; i < texto.size(); i++) {
        if (texto[i] == separador) {
            partes.push_back(actual);
            actual.clear();
        } else {
            actual.push_back(texto[i]);
        }
    }
    partes.push_back(actual);
    return partes;
}

// Texto con saltos de linea y ajuste automatico al ancho del rectangulo
void Aplicacion::textoAjustado(const std::string& texto, Rectangle area, int tamano, Color color) {
    int altoLinea = tamano + 5;
    float y = area.y;
    std::vector<std::string> parrafos = separar(texto, '\n');
    for (size_t p = 0; p < parrafos.size(); p++) {
        std::vector<std::string> palabras = separar(parrafos[p], ' ');
        std::string linea;
        for (size_t i = 0; i < palabras.size(); i++) {
            std::string prueba = linea.empty() ? palabras[i] : linea + " " + palabras[i];
            if (!linea.empty() && MeasureText(prueba.c_str(), tamano) > area.width) {
                if (y + tamano <= area.y + area.height + 2) DrawText(linea.c_str(), static_cast<int>(area.x), static_cast<int>(y), tamano, color);
                y += altoLinea;
                linea = palabras[i];
            } else {
                linea = prueba;
            }
        }
        if (y + tamano <= area.y + area.height + 2) DrawText(linea.c_str(), static_cast<int>(area.x), static_cast<int>(y), tamano, color);
        y += altoLinea;
    }
}

// Dibuja todos los botones de la pantalla actual (con efecto al pasar el raton)
void Aplicacion::dibujarBotones() {
    Vector2 raton = GetMousePosition();
    for (size_t i = 0; i < botones.size(); i++) {
        const Boton& b = botones[i];
        Color relleno = C_BOTON_OFF;
        if (b.habilitado) relleno = CheckCollisionPointRec(raton, b.area) ? C_BOTON_HOVER : C_BOTON;
        rectRedondeado(b.area, relleno, relleno, 14);
        int tamano = tamanoParaAncho(b.texto, static_cast<int>(b.area.width) - 16, 20);
        textoCentrado(b.texto, b.area, tamano, b.habilitado ? C_BLANCO : C_TEXTO_SUAVE, true);
    }
}

// Geometria: celda 'posicion' de un tablero cuadrado de lado 'tam'
Rectangle Aplicacion::rectCelda(int x0, int y0, int tam, int posicion) {
    const int separacion = 8;
    int lado = (tam - 4 * separacion) / 3;
    int columna = posicion % 3;
    int fila = posicion / 3;
    return Rectangle{ static_cast<float>(x0 + separacion + columna * (lado + separacion)),
                      static_cast<float>(y0 + separacion + fila * (lado + separacion)),
                      static_cast<float>(lado), static_cast<float>(lado) };
}

// Devuelve que casilla (0..8) esta bajo el punto (px, py) o -1
int Aplicacion::posicionDeClic(int x0, int y0, int tam, int px, int py) {
    for (int p = 0; p < 9; p++) {
        Rectangle r = rectCelda(x0, y0, tam, p);
        if (px >= r.x && px < r.x + r.width && py >= r.y && py < r.y + r.height) return p;
    }
    return -1;
}

// Dibuja un tablero. 'resaltada' se pinta dorada (sugerencia), 'seleccionada' con borde azul.
void Aplicacion::dibujarTablero(const Tablero& tablero, int x0, int y0, int tam, int tamanoFuente,
                                int resaltada, int seleccionada) {
    Rectangle panel = { static_cast<float>(x0), static_cast<float>(y0), static_cast<float>(tam), static_cast<float>(tam) };
    rectRedondeado(panel, C_PANEL, C_BORDE, 22);
    for (int p = 0; p < 9; p++) {
        Rectangle celda = rectCelda(x0, y0, tam, p);
        int valor = tablero.obtenerFicha(p);
        if (valor == 0) {
            rectRedondeado(celda, C_HUECO, (p == seleccionada) ? C_AZUL : C_HUECO, 14);
            continue;
        }
        Color relleno = (p == resaltada) ? C_DORADO : C_FICHA;
        Color borde = (p == seleccionada) ? C_AZUL : relleno;
        rectRedondeado(celda, relleno, borde, 14);
        textoCentrado(std::to_string(valor), celda, tamanoFuente, C_TEXTO_FICHA, true);
    }
}

// ---- Pantalla: menu principal ----
void Aplicacion::dibujarMenu() {
    textoCentrado("8-PUZZLE", Rectangle{ 0, 40, 760, 60 }, 56, C_DORADO, false);
    textoCentrado("Estructuras de Datos II  -  Proyecto Final", Rectangle{ 0, 105, 760, 30 }, 20, C_TEXTO_SUAVE, false);
    textoCentrado("Algoritmo A* con distancia de Manhattan", Rectangle{ 0, 535, 760, 30 }, 16, C_TEXTO_SUAVE, false);
}

// ---- Pantalla: capturar alias ----
void Aplicacion::dibujarAlias() {
    textoCentrado("Modo Manual", Rectangle{ 0, 60, 760, 60 }, 46, C_DORADO, false);
    textoCentrado("Escribe tu alias (maximo 12 letras o numeros):", Rectangle{ 0, 170, 760, 30 }, 20, C_TEXTO, false);
    Rectangle caja = { 230, 225, 300, 60 };
    rectRedondeado(caja, C_PANEL, C_AZUL, 14);
    textoCentrado(alias + "_", caja, 26, C_TEXTO, true);
    textoCentrado(mensaje, Rectangle{ 0, 292, 760, 24 }, 16, C_ROJO, false);
}

// Tarjeta con el nombre y las reglas de un nivel
void Aplicacion::dibujarTarjetaNivel(int numeroNivel, Rectangle area) {
    std::unique_ptr<Nivel> nivel = crearNivel(numeroNivel);
    if (!nivel) return;
    rectRedondeado(area, C_PANEL, C_BORDE, 20);
    textoIzquierda(nivel->obtenerNombre(), static_cast<int>(area.x) + 15, static_cast<int>(area.y) + 14, 26, C_DORADO);
    Rectangle texto = { area.x + 15, area.y + 62, area.width - 30, area.height - 72 };
    textoAjustado(nivel->describirReglas(), texto, 16, C_TEXTO);
}

// ---- Pantalla: elegir nivel / reglas ----
void Aplicacion::dibujarElegirNivel(bool conBotones) {
    textoCentrado(conBotones ? "Elige el nivel" : "Reglas de los niveles", Rectangle{ 0, 30, 760, 60 }, 46, C_DORADO, false);
    dibujarTarjetaNivel(1, Rectangle{ 40, 100, 320, 285 });
    dibujarTarjetaNivel(2, Rectangle{ 400, 100, 320, 285 });
    if (!conBotones) {
        textoCentrado("Meta en ambos niveles: 1 2 3 / 4 5 6 / 7 8 _  (los puntos se acumulan por alias)",
                      Rectangle{ 0, 405, 760, 24 }, 16, C_TEXTO_SUAVE, false);
    }
}

// ---- Pantalla: partida manual ----
void Aplicacion::dibujarManual() {
    textoIzquierda("Modo Manual", 30, 25, 26, C_DORADO);
    dibujarTablero(partida.obtenerTablero(), 30, 80, 440, 64, partida.obtenerFichaSugerida(), -1);

    const Nivel* nivel = partida.obtenerNivel();
    if (nivel == nullptr) return;

    int x = 500;
    int y = 80;
    textoIzquierdaAjustado("Jugador: " + alias, x, y, 240, 20, C_TEXTO);
    textoIzquierdaAjustado(nivel->obtenerNombre(), x, y + 28, 240, 20, C_TEXTO);

    std::string textoMovs = "Movimientos: " + std::to_string(partida.obtenerMovimientos());
    if (nivel->obtenerLimiteMovimientos() > 0) textoMovs += " / " + std::to_string(nivel->obtenerLimiteMovimientos());
    textoIzquierdaAjustado(textoMovs, x, y + 56, 240, 20, C_TEXTO);

    std::string textoPistas = "Pistas usadas: " + std::to_string(partida.obtenerPistasUsadas());
    if (nivel->obtenerPistasPermitidas() >= 0) textoPistas += " / " + std::to_string(nivel->obtenerPistasPermitidas());
    textoIzquierdaAjustado(textoPistas, x, y + 84, 240, 20, C_TEXTO);

    textoIzquierdaAjustado("Tiempo: " + formatearTiempo(partida.obtenerSegundos()), x, y + 112, 240, 20, C_TEXTO);

    // Mensaje de estado (verde si ganaste, rojo si perdiste)
    Color colorMensaje = C_DORADO;
    if (partida.obtenerEstado() == EstadoPartida::GANADA) colorMensaje = C_VERDE;
    if (partida.obtenerEstado() == EstadoPartida::PERDIDA) colorMensaje = C_ROJO;
    textoAjustado(mensaje, Rectangle{ static_cast<float>(x), static_cast<float>(y + 150), 240, 105 }, 16, colorMensaje);

    textoIzquierda("Meta: 1 2 3 / 4 5 6 / 7 8 _", 30, 530, 16, C_TEXTO_SUAVE);
}

// ---- Pantalla: configurar modo inteligente ----
void Aplicacion::dibujarConfigInteligente() {
    textoIzquierda("Modo Inteligente", 30, 15, 26, C_DORADO);
    textoIzquierda("Haz clic en dos casillas del mismo tablero para intercambiarlas.", 30, 55, 16, C_TEXTO_SUAVE);
    textoIzquierda("INICIO", 30, 85, 20, C_TEXTO);
    textoIzquierda("META", 400, 85, 20, C_TEXTO);

    dibujarTablero(sesion.obtenerInicio(), 30, 115, 320, 44, -1, (seleccionTablero == 1) ? seleccionPosicion : -1);
    dibujarTablero(sesion.obtenerMeta(), 400, 115, 320, 44, -1, (seleccionTablero == 2) ? seleccionPosicion : -1);

    // Flecha entre los tableros
    textoCentrado(">", Rectangle{ 350, 235, 50, 80 }, 46, C_TEXTO_SUAVE, true);

    textoAjustado(mensaje, Rectangle{ 30, 442, 700, 32 }, 16, C_DORADO);
}

// ---- Pantalla: solucion paso a paso ----
void Aplicacion::dibujarReproduccion() {
    textoIzquierda("Solucion paso a paso (A*)", 30, 25, 26, C_DORADO);
    dibujarTablero(sesion.obtenerTableroActual(), 30, 80, 440, 64, -1, -1);

    int x = 500;
    int y = 80;
    textoIzquierdaAjustado("Paso " + std::to_string(sesion.obtenerPasoActual()) + " de " +
                               std::to_string(sesion.obtenerTotalPasos()),
                           x, y, 240, 26, C_TEXTO);
    textoAjustado(sesion.describirPaso(sesion.obtenerPasoActual()),
                  Rectangle{ static_cast<float>(x), static_cast<float>(y + 42), 240, 60 }, 20, C_TEXTO);
    textoIzquierdaAjustado("Nodos expandidos: " + std::to_string(sesion.obtenerNodosExpandidos()), x, y + 110, 240, 16, C_TEXTO_SUAVE);
    textoIzquierda("Heuristica: Manhattan", x, y + 134, 16, C_TEXTO_SUAVE);

    if (sesion.obtenerPasoActual() == sesion.obtenerTotalPasos()) {
        textoIzquierdaAjustado("META ALCANZADA", x, y + 170, 240, 26, C_VERDE);
    }
}

// ---- Pantalla: reporte de puntuaciones (mayor a menor) ----
void Aplicacion::dibujarPuntajes() {
    textoCentrado("Puntuaciones", Rectangle{ 0, 25, 760, 60 }, 46, C_DORADO, false);

    textoIzquierda("#", 120, 95, 20, C_TEXTO_SUAVE);
    textoIzquierda("Jugador", 170, 95, 20, C_TEXTO_SUAVE);
    textoIzquierda("Puntos", 400, 95, 20, C_TEXTO_SUAVE);
    textoIzquierda("Ultima partida", 520, 95, 20, C_TEXTO_SUAVE);

    if (reporte.empty()) {
        textoCentrado("Todavia no hay puntuaciones. Gana una partida manual!", Rectangle{ 0, 250, 760, 30 }, 20, C_TEXTO, false);
        return;
    }
    size_t limite = reporte.size() < 12 ? reporte.size() : 12;  // caben 12 renglones
    for (size_t i = 0; i < limite; i++) {
        int y = 130 + static_cast<int>(i) * 31;
        Color color = (i == 0) ? C_DORADO : C_TEXTO;
        textoIzquierda(std::to_string(i + 1), 120, y, 20, color);
        textoIzquierdaAjustado(reporte[i].alias, 170, y, 215, 20, color);
        textoIzquierda(std::to_string(reporte[i].puntos), 400, y, 20, color);
        textoIzquierda(reporte[i].fecha, 520, y, 20, color);
    }
}

// ======================================================================
//  Utilidades y sonido
// ======================================================================

// Convierte segundos a "mm:ss"
std::string Aplicacion::formatearTiempo(int segundos) {
    int minutos = segundos / 60;
    int resto = segundos % 60;
    std::string texto = std::to_string(minutos) + ":";
    if (resto < 10) texto += "0";
    texto += std::to_string(resto);
    return texto;
}

// Genera un sonido en memoria a partir de una lista de (frecuencia Hz, duracion ms)
Sound Aplicacion::crearSonido(const std::vector<std::pair<int, int>>& tonos) {
    const int frecuenciaMuestreo = 22050;
    const double pi = 3.14159265358979323846;
    std::vector<short> muestras;
    for (size_t t = 0; t < tonos.size(); t++) {
        int cantidad = frecuenciaMuestreo * tonos[t].second / 1000;
        for (int i = 0; i < cantidad; i++) {
            // Entrada y salida suaves de 5 ms para evitar "clics" en el audio
            double envolvente = 1.0;
            int borde = frecuenciaMuestreo / 200;
            if (i < borde) envolvente = static_cast<double>(i) / borde;
            if (cantidad - i < borde) envolvente = static_cast<double>(cantidad - i) / borde;
            double onda = std::sin(2.0 * pi * tonos[t].first * i / frecuenciaMuestreo);
            muestras.push_back(static_cast<short>(onda * envolvente * 0.3 * 32767));
        }
    }
    Wave ondaSonora = {};
    ondaSonora.frameCount = static_cast<unsigned int>(muestras.size());
    ondaSonora.sampleRate = frecuenciaMuestreo;
    ondaSonora.sampleSize = 16;
    ondaSonora.channels = 1;
    ondaSonora.data = muestras.data();
    return LoadSoundFromWave(ondaSonora);  // raylib copia los datos, el vector se libera solo
}

void Aplicacion::iniciarAudio() {
    InitAudioDevice();
    audioListo = IsAudioDeviceReady();
    if (!audioListo) return;
    sonidoMover = crearSonido({ { 600, 40 } });
    sonidoFallo = crearSonido({ { 200, 120 } });
    sonidoAyuda = crearSonido({ { 880, 60 }, { 1100, 60 } });
    sonidoGanar = crearSonido({ { 523, 120 }, { 659, 120 }, { 784, 120 }, { 1047, 250 } });
}

void Aplicacion::cerrarAudio() {
    if (!audioListo) return;
    UnloadSound(sonidoMover);
    UnloadSound(sonidoFallo);
    UnloadSound(sonidoAyuda);
    UnloadSound(sonidoGanar);
    CloseAudioDevice();
    audioListo = false;
}

void Aplicacion::reproducir(const Sound& sonido) {
    if (audioListo) PlaySound(sonido);
}

void Aplicacion::sonidoMovimiento() { reproducir(sonidoMover); }
void Aplicacion::sonidoError() { reproducir(sonidoFallo); }
void Aplicacion::sonidoPista() { reproducir(sonidoAyuda); }
void Aplicacion::sonidoVictoria() { reproducir(sonidoGanar); }

// ======================================================================
//  Punto de entrada
// ======================================================================

int main() {
    Aplicacion aplicacion;
    return aplicacion.ejecutar();
}
