#include "Enemigo.h"
#include "Pantalla.h"

Enemigo::Enemigo(int startX, int startY) : xpos(startX), ypos(startY) {}

void Enemigo::moveDown(int limite) {
    if (ypos < limite - 1) ypos++;
}

void Enemigo::moveUp(int limite) {
    if (ypos > 0) ypos--;
}

void Enemigo::moveLeft(int limite) {
    if (xpos > 0) xpos--;
}

void Enemigo::moveRight(int limite) {
    if (xpos < limite - 1) xpos++;
}
void Enemigo::draw(Pantalla& p) {
    p.dibujar(xpos, ypos, 'X');
}
int Enemigo::getX() const { return xpos; }
int Enemigo::getY() const { return ypos; }

void avoidCollision(Enemigo& e1, Enemigo& e2) {
    if (e1.getX() == e2.getX() && e1.getY() == e2.getY()) {
        // Si colisionan, mover uno de ellos hacia la izquierda si es posible
        e1.moveLeft(25); // Asumiendo un límite de 25 para el eje X
    }
}