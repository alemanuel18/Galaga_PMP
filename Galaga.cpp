#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "Pantalla.h"
#include "Nave.h"

using namespace std;

// ---------------- FUNCIONES AUXILIARES PARA LINUX ----------------

// getch() versión Linux
int getchLinux()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // sin buffer, sin eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// kbhit() versión Linux
int kbhit()
{
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Mover cursor
void gotoxy(int x, int y)
{
    printf("\033[%d;%dH", y + 1, x + 1);
}

// Colores ANSI
void setColor(int color)
{
    switch (color)
    {
    case 7:
        printf("\033[0m");
        break; // Normal
    case 10:
        printf("\033[92m");
        break; // Verde brillante
    case 11:
        printf("\033[96m");
        break; // Cyan brillante
    case 13:
        printf("\033[95m");
        break; // Magenta
    case 14:
        printf("\033[93m");
        break; // Amarillo brillante
    case 15:
        printf("\033[97m");
        break; // Blanco brillante
    default:
        printf("\033[0m");
        break; // Normal
    }
}

// Ocultar cursor
void hideCursor()
{
    printf("\033[?25l");
}

// Mostrar cursor
void showCursor()
{
    printf("\033[?25h");
}

// Limpiar pantalla
void clearScreen()
{
    printf("\033[2J\033[H");
}

// ---------------- INTERFAZ VISUAL ----------------

// Marco decorativo
void drawFrame()
{
    setColor(11); // Cyan brillante
    // Marco superior
    gotoxy(0, 0);
    for (int i = 0; i < 80; i++)
        cout << "═";

    // Marco inferior
    gotoxy(0, 24);
    for (int i = 0; i < 80; i++)
        cout << "═";

    // Marco lateral
    for (int i = 1; i < 24; i++)
    {
        gotoxy(0, i);
        cout << "║";
        gotoxy(79, i);
        cout << "║";
    }

    // Esquinas
    gotoxy(0, 0);
    cout << "╔";
    gotoxy(79, 0);
    cout << "╗";
    gotoxy(0, 24);
    cout << "╚";
    gotoxy(79, 24);
    cout << "╝";
}

// Pantalla de inicio
void showSplashScreen()
{
    clearScreen();
    drawFrame();

    setColor(15); // Blanco brillante
    gotoxy(15, 8);
    cout << "  ██████   █████  ██       █████   ██████   █████ ";
    gotoxy(15, 9);
    cout << " ██       ██   ██ ██      ██   ██ ██       ██   ██";
    gotoxy(15, 10);
    cout << " ██   ███ ███████ ██      ███████ ██   ███ ███████";
    gotoxy(15, 11);
    cout << " ██    ██ ██   ██ ██      ██   ██ ██    ██ ██   ██";
    gotoxy(15, 12);
    cout << "  ██████  ██   ██ ███████ ██   ██  ██████  ██   ██";

    setColor(10);
    gotoxy(20, 18);
    cout << "Presiona cualquier tecla para continuar...";

    setColor(7);
    getchLinux();
}

// Pantalla de puntajes
void showScoresScreen()
{
    clearScreen();
    drawFrame();

    setColor(11);
    gotoxy(32, 3);
    cout << "MEJORES PUNTAJES";

    setColor(14);
    gotoxy(30, 4);
    cout << "━━━━━━━━━━━━━━━━━━━━";
    gotoxy(33, 5);
    cout << "Datos de prueba";

    setColor(15);
    gotoxy(25, 8);
    cout << "1. COMANDANTE ACE ........... 15,430";
    gotoxy(25, 10);
    cout << "2. PILOTO NOVA .............. 12,850";
    gotoxy(25, 12);
    cout << "3. CAPITÁN STELLAR .......... 11,200";
    gotoxy(25, 14);
    cout << "4. TENIENTE COSMOS .......... 9,750";
    gotoxy(25, 16);
    cout << "5. CADETE SPACE ............. 8,320";

    setColor(10);
    gotoxy(24, 20);
    cout << "Presiona cualquier tecla para regresar...";

    setColor(7);
    getchLinux();
}

// ---------------- GAME LOOP ----------------
void gameLoop()
{
    Pantalla pantalla(80, 25);
    Nave nave(pantalla.getAncho() / 2, pantalla.getAlto() - 2);

    bool running = true;
    while (running)
    {
        pantalla.limpiar();
        nave.dibujar(pantalla);
        pantalla.mostrar();

        if (kbhit())
        {
            int tecla = getchLinux();
            switch (tecla)
            {
            case 'a':
            case 'A':
                nave.moverIzquierda();
                break;
            case 'd':
            case 'D':
                nave.moverDerecha(pantalla.getAncho());
                break;
            case 'q':
            case 'Q':
                running = false;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

// ---------------- MAIN ----------------
int main()
{
    hideCursor();
    showSplashScreen();

    while (true)
    {
        clearScreen();
        drawFrame();
        setColor(11);
        gotoxy(34, 3);
        cout << "G A L A G A";
        setColor(14);
        gotoxy(29, 4);
        cout << "━━━━━━━━━━━━━━━━━━━━━";

        setColor(15);
        gotoxy(30, 19);
        cout << "1. Iniciar Partida";
        gotoxy(30, 20);
        cout << "2. Puntajes";
        gotoxy(30, 21);
        cout << "3. Salir";

        setColor(10);
        gotoxy(27, 23);
        cout << "Selecciona una opción: ";
        setColor(7);

        char choice = getchLinux();
        switch (choice)
        {
        case '1':
            gameLoop();
            break;
        case '2':
            showScoresScreen();
            break;
        case '3':
            clearScreen();
            gotoxy(30, 12);
            setColor(14);
            cout << "¡Gracias por jugar GALAGA!";
            gotoxy(0, 25);
            setColor(7);
            showCursor();
            return 0;
        }
    }
}
