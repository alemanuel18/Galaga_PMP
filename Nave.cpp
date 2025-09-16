#include "Nave.h"

Nave::Nave(int startX, int startY) : x(startX), y(startY) {}

void Nave::moverIzquierda() {
    if (x > 0) x--;
}

void Nave::moverDerecha(int limite) {
    if (x < limite - 1) x++;
}

void Nave::dibujar(Pantalla& p) {
    p.dibujar(x, y, 'A'); // símbolo de la nave
}

int Nave::getX() const { return x; }
int Nave::getY() const { return y; }
