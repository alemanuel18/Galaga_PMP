# 🎮 Proyecto GALAGA (Versión Consola en C++)

Este proyecto es una implementación en **C++** de un menú interactivo inspirado en el clásico juego **Galaga**, utilizando la terminal para mostrar gráficos ASCII y colores ANSI.

## ✨ Características

- 🎨 **Pantallas decorativas** con marcos y colores.
- 🚀 **Pantalla de inicio (Splash Screen)** con arte ASCII del logo de GALAGA.
- 🎮 **Controles representados con iconos ASCII** (mover izquierda, derecha, disparar).
- ⭐ **Objetivo del juego** explicado en pantalla.
- 📋 **Menú principal**:
  - Iniciar partida
  - Ver puntajes
  - Salir
- ⚔️ **Modos de juego disponibles**:
  - **Modo 1**: 40 alienígenas en 5 grupos de 8.
  - **Modo 2**: 50 alienígenas en 5 grupos de 10.
  - **Modo 3**: 60 alienígenas en 6 grupos de 10.
- 🏆 **Pantalla de puntajes** (con datos de ejemplo).
- 👾 Estructura lista para agregar la lógica del juego.

## 📂 Estructura del código

- **Funciones de consola**: manejo de cursor, colores y pantalla.
- **Pantallas principales**: inicio, menú, objetivos, controles y puntajes.
- **Menú de navegación**: permite moverse entre opciones sin necesidad de presionar *Enter*.
- **Estructura de bucle principal**: mantiene activo el menú hasta que el jugador elija salir.

## 🛠️ Requisitos

- Sistema operativo **Linux / macOS** (usa `termios` y `unistd`).
- Compilador de C++ compatible con **C++11 o superior** (ej: `g++`).

## ▶️ Cómo compilar y ejecutar

```bash
# Compilar
g++ Galaga.cpp -o Galaga.exe

# Ejecutar
./Galaga.exe
```

## 📸 Vista Previa (ASCII Art)

Pantalla de inicio:
```bash
                     ██████   █████  ██      █████   ██████   █████ 
                    ██       ██   ██ ██     ██   ██ ██       ██   ██
                    ██   ███ ███████ ██     ███████ ██   ███ ███████
                    ██    ██ ██   ██ ██     ██   ██ ██    ██ ██   ██
                     ██████  ██   ██ ██████ ██   ██  ██████  ██   ██

```

## 🚧 Próximos pasos

 - Implementar la lógica del juego (movimiento de la nave, disparos, oleadas de enemigos).
 - Guardar puntajes en archivo externo.

## 👨‍💻 Autores

 - Marcelo Detlefsen - 24554
 - Julián Divas - 24687
 - Marco Diáz - 24229
 - Alejandro Jeréz - 24678