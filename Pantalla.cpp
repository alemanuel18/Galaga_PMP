#include "Pantalla.h"

Pantalla::Pantalla(int w, int h) : ancho(w), alto(h) {
    initscr();            // iniciar ncurses
    noecho();             // no mostrar las teclas
    curs_set(0);          // ocultar cursor
    keypad(stdscr, TRUE); // habilitar teclas especiales
    nodelay(stdscr, TRUE); // input no bloqueante
    resize_term(h, w);
}

Pantalla::~Pantalla() {
    endwin(); // cerrar ncurses
}

void Pantalla::limpiar() {
    clear();
}

void Pantalla::dibujar(int x, int y, char c) {
    if (x >= 0 && x < ancho && y >= 0 && y < alto) {
        mvaddch(y, x, c);
    }
}

void Pantalla::mostrar() {
    refresh();
}

int Pantalla::getAncho() const { return ancho; }
int Pantalla::getAlto() const { return alto; }
