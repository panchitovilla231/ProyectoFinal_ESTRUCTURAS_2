cmake_minimum_required(VERSION 3.14)
project(Puzzle8 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- raylib: se usa la instalada (por ejemplo con Homebrew) o se descarga ----
find_package(raylib 5.0 QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# ---- Logica del juego (C++ estandar, sin dependencias graficas) ----
set(NUCLEO
    Tablero.cpp
    Heuristica.cpp
    BuscadorAEstrella.cpp
    Nivel.cpp
    Puntuaciones.cpp
    Juego.cpp
)

# ---- El juego ----
add_executable(puzzle8 ${NUCLEO} Interfaz.cpp)
target_link_libraries(puzzle8 PRIVATE raylib)

# En Windows: que no abra la ventana negra de consola junto al juego
if (WIN32)
    if (MSVC)
        target_link_options(puzzle8 PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
    else()
        target_link_options(puzzle8 PRIVATE -mwindows)
    endif()
endif()

# ---- Pruebas de consola de la logica (opcional: cmake --build build --target test_nucleo) ----
add_executable(test_nucleo EXCLUDE_FROM_ALL ${NUCLEO} test_nucleo.cpp)
