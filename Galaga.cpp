#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <string>
#include "Pantalla.h"
#include "Nave.h"

using namespace std;

// Estructura para los puntajes
struct Score {
    string name;
    int points;
};

// Vector global para almacenar puntajes
vector<Score> highScores;

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
    case 12:
        printf("\033[91m");
        break; // Rojo brillante
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

// Función para obtener el nombre del jugador
string getPlayerName() {
    clearScreen();
    drawFrame();
    
    setColor(14);
    gotoxy(30, 10);
    cout << "¡GAME OVER!";
    
    setColor(15);
    gotoxy(25, 12);
    cout << "Ingresa tu nombre (max 15 chars): ";
    
    // Mostrar cursor para escribir
    showCursor();
    setColor(10);
    
    string name;
    char c;
    gotoxy(25, 14);
    
    // Leer caracteres uno por uno
    while (true) {
        c = getchLinux();
        
        if (c == '\n' || c == '\r') { // Enter
            break;
        } else if (c == 127 || c == 8) { // Backspace
            if (name.length() > 0) {
                name.pop_back();
                gotoxy(25, 14);
                cout << string(20, ' '); // Limpiar línea
                gotoxy(25, 14);
                cout << name;
            }
        } else if (c >= 32 && c <= 126 && name.length() < 15) { // Caracteres imprimibles
            name += c;
            cout << c;
        }
    }
    
    hideCursor();
    
    if (name.empty()) {
        name = "ANONIMO";
    }
    
    return name;
}

// Pantalla de puntajes actualizada
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

    setColor(15);
    
    if (highScores.empty()) {
        gotoxy(30, 12);
        cout << "No hay puntajes aún";
    } else {
        // Ordenar puntajes de mayor a menor
        sort(highScores.begin(), highScores.end(), 
             [](const Score& a, const Score& b) { return a.points > b.points; });
        
        int y = 7;
        for (int i = 0; i < min(10, (int)highScores.size()); i++) {
            gotoxy(20, y);
            cout << (i + 1) << ". " << highScores[i].name;
            
            // Puntos alineados a la derecha
            string pointsStr = to_string(highScores[i].points);
            gotoxy(60 - pointsStr.length(), y);
            cout << pointsStr;
            
            y += 2;
        }
    }

    setColor(10);
    gotoxy(24, 22);
    cout << "Presiona cualquier tecla para regresar...";

    setColor(7);
    getchLinux();
}

// ---------------- GAME LOOP  ----------------
void gameLoop()
{
    clearScreen();
    drawFrame();
    
    setColor(15);
    gotoxy(30, 8);
    cout << "SIMULADOR GALAGA";
    
    setColor(14);
    gotoxy(20, 12);
    cout << "S - Sumar puntos (+100)";
    gotoxy(20, 14);
    cout << "M - Morir (terminar juego)";
    gotoxy(20, 16);
    cout << "Q - Salir sin puntaje";
    
    setColor(10);
    gotoxy(25, 20);
    cout << "Presiona una tecla para comenzar...";
    
    getchLinux(); // Esperar tecla para empezar
    
    int score = 0;
    bool gameRunning = true;

    while (gameRunning)
    {
        clearScreen();
        drawFrame();
        
        setColor(15);
        gotoxy(30, 8);
        cout << "PUNTAJE: " << score;
        
        setColor(14);
        gotoxy(20, 12);
        cout << "S - Sumar puntos (+100)";
        gotoxy(20, 14);
        cout << "M - Morir (terminar juego)";
        gotoxy(20, 16);
        cout << "Q - Salir sin puntaje";

        if (kbhit())
        {
            int tecla = getchLinux();
            switch (tecla)
            {
            case 's':
            case 'S':
                score += 100;
                break;
                
            case 'm':
            case 'M':
                // Terminar juego y pedir nombre
                if (score > 0) {
                    string playerName = getPlayerName();
                    Score newScore;
                    newScore.name = playerName;
                    newScore.points = score;
                    highScores.push_back(newScore);
                    showScoresScreen();
                }
                gameRunning = false;
                break;
                
            case 'q':
            case 'Q':
                gameRunning = false;
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Función para mostrar la nave en el menú
void showMenu() {
    Pantalla pantalla(80, 25);
    Nave nave(pantalla.getAncho() / 2, 15); // Posición más arriba para el menú

    while (true)
    {
        clearScreen();
        drawFrame();
        
        // Título
        setColor(11);
        gotoxy(34, 3);
        cout << "G A L A G A";
        setColor(14);
        gotoxy(29, 4);
        cout << "━━━━━━━━━━━━━━━━━━━━━";

        // Opciones del menú
        setColor(15);
        gotoxy(30, 19);
        cout << "1. Iniciar Partida";
        gotoxy(30, 20);
        cout << "2. Puntajes";
        gotoxy(30, 21);
        cout << "3. Salir";

        setColor(10);
        gotoxy(25, 23);
        cout << "A/D para mover nave - Número para seleccionar";
        setColor(7);

        // Dibujar la nave
        setColor(10);
        gotoxy(nave.getX(), nave.getY());
        cout << "A";

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
            case '1':
                return; // Salir al gameLoop
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
                exit(0);
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
        showMenu();
        gameLoop(); // Solo se ejecuta cuando se presiona '1'
    }
}