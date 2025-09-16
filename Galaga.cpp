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
#include "Enemigo.h"

using namespace std;

// Estructura para los puntajes
struct Score
{
    string name;
    int points;
};

// Vector global para almacenar puntajes
vector<Score> highScores;

// ---------------- FUNCIONES AUXILIARES PARA LINUX ----------------

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
// Función para mostrar iconos de control
void drawControlIcons()
{
    setColor(13);
    // Magenta // Icono mover izquierda
    gotoxy(5, 8);
    cout << "┌─────────────┐";
    gotoxy(5, 9);
    cout << "│  ← Mover ←  │";
    gotoxy(5, 10);
    cout << "│      A      │";
    gotoxy(5, 11);
    cout << "└─────────────┘"; // Icono mover derecha
    gotoxy(5, 13);
    cout << "┌─────────────┐";
    gotoxy(5, 14);
    cout << "│  → Mover →  │";
    gotoxy(5, 15);
    cout << "│     D       │";
    gotoxy(5, 16);
    cout << "└─────────────┘"; // Icono disparar
    gotoxy(59, 8);
    cout << "┌──────────────┐";
    gotoxy(59, 9);
    cout << "│ ↑ DISPARAR ↑ │";
    gotoxy(59, 10);
    cout << "│   [______]   │";
    gotoxy(59, 11);
    cout << "│   [ESPACIO]  │";
    gotoxy(59, 12);

    cout << "└──────────────┘"; // Nave del jugador
    setColor(11);
    
    gotoxy(60, 13);
    cout << "┌─────────────┐";
    gotoxy(60, 14);
    cout << "│     NAVE    │";
    gotoxy(60, 15);
    cout << "│      A      │";
    gotoxy(60, 16);
    cout << "└─────────────┘";
  
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
string getPlayerName()
{
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
    while (true)
    {
        c = getchLinux();

        if (c == '\n' || c == '\r')
        { // Enter
            break;
        }
        else if (c == 127 || c == 8)
        { // Backspace
            if (name.length() > 0)
            {
                name.pop_back();
                gotoxy(25, 14);
                cout << string(20, ' '); // Limpiar línea
                gotoxy(25, 14);
                cout << name;
            }
        }
        else if (c >= 32 && c <= 126 && name.length() < 15)
        { // Caracteres imprimibles
            name += c;
            cout << c;
        }
    }

    hideCursor();

    if (name.empty())
    {
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

    if (highScores.empty())
    {
        gotoxy(30, 12);
        cout << "No hay puntajes aún";
    }
    else
    {
        // Ordenar puntajes de mayor a menor
        sort(highScores.begin(), highScores.end(),
             [](const Score &a, const Score &b)
             { return a.points > b.points; });

        int y = 7;
        for (int i = 0; i < min(10, (int)highScores.size()); i++)
        {
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
void gameScreen()
{
    // Variables del juego
    bool gameRunning = true;
    int score = 0;
    int naveX = 40;
    int naveY = 20;

    
    // Posiciones de enemigos
    vector<pair<int, int>> enemigos = {
        {15, 8}, {20, 8}, {25, 8}, {30, 8}, {35, 8}, {40, 8}, {45, 8}, {50, 8}, {55, 8}
    };

    // Controlar flujo de la partida
    bool inGame = false;

    clearScreen();
    drawFrame();
    hideCursor();

    setColor(15);
    gotoxy(30, 8);
    cout << "GALAGA - MODO JUEGO";

    setColor(14);
    gotoxy(15, 10);
    cout << "ESPACIO - Disparar y ganar puntos (+100)";
    gotoxy(15, 12);
    cout << "A/D - Mover nave";
    gotoxy(15, 14);
    cout << "M - Terminar partida y guardar puntaje";
    gotoxy(15, 16);
    cout << "Q - Salir sin guardar.";
    gotoxy(15, 18);
    cout << "Destruye todas las naves!!!";


    setColor(10);
    gotoxy(20, 21);
    cout << "Presiona cualquier tecla para comenzar...";

    getchLinux();
    inGame = true;

    while (gameRunning) 
    {
        if (inGame) {
            // Pantalla de juego activa con nave y enemigos
            clearScreen();
            drawFrame();
            
            setColor(15);
            gotoxy(25, 1);
            cout << "GALAGA - PUNTAJE: " << score;
            
            setColor(14);
            gotoxy(2, 1);
            cout << "VIDAS: ♥♥♥";

            // Crear enemigos
            setColor(12);
            for (auto &enemigo : enemigos) {
                gotoxy(enemigo.first, enemigo.second);
                cout << "X";
            }

            //Ubicar nave
            setColor(10);
            gotoxy(naveX, naveY);
            cout << "A";

            // Instrucciones de juego
            setColor(11);
            gotoxy(2, 22);
            cout << "A/D: Mover - ESPACIO: Disparar - M: Terminar - Q: Salir ";
            gotoxy(2, 23);
            cout << "Destruye todas las naves!!!";
            
            setColor(7);

        } else {
            clearScreen();
            drawFrame();

            setColor(15);
            gotoxy(30, 8);
            cout << "PUNTAJE ACTUAL: " << score;

            setColor(14);
            gotoxy(20, 12);
            cout << "ESPACIO - Continuar jugando";
            gotoxy(20, 14);
            cout << "M - Terminar y guardar puntaje";
            gotoxy(20, 16);
            cout << "Q - Salir sin guardar";
        }

        // Manejo de controles
        if (kbhit()) {
            int tecla = getchLinux();
            
            if (inGame) {
                switch (tecla) {
                    case 'a': case 'A':
                        if (naveX > 2) naveX--;
                        break;
                    case 'd': case 'D':
                        if (naveX < 77) naveX++;
                        break;
                    case ' ': // Disparar
                        score += 100;
                        break;
                    case 'm': case 'M':
                        // Terminar juego y guardar
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
                    case 'q': case 'Q':
                        gameRunning = false;
                        break;
                    case 'p': case 'P':
                        inGame = false;
                        break;
                }
            } else {
                // Controles en pausa
                switch (tecla) {
                    case ' ': // Continuar
                        inGame = true;
                        break;
                    case 'm': case 'M':
                        // Guardar y salir
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
                    case 'q': case 'Q':
                        gameRunning = false;
                        break;
                }
            }
        }

        // Evitar parpadeo
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Función para mostrar la nave en el menú
int showMenu(){
    Pantalla pantalla(80, 25);
    Nave nave(pantalla.getAncho() / 2, 15);
    int lastNaveX = nave.getX(); // Guardar posición anterior
    bool needRedraw = true;      // Flag para controlar cuándo redibujar

    while (true)
    {
        // Solo redibujar si es necesario
        if (needRedraw || nave.getX() != lastNaveX) {
            clearScreen();
            drawFrame();
            setColor(11);
            gotoxy(34, 3);
            cout << "G A L A G A";
            setColor(14);
            gotoxy(29, 4);
            cout << "━━━━━━━━━━━━━━━━━━━━━";
            drawControlIcons();

            setColor(15);
            gotoxy(26, 17); cout << "1. Iniciar Partida dificultad 1";
            gotoxy(26, 18); cout << "2. Iniciar Partida dificultad 2";
            gotoxy(26, 19); cout << "3. Iniciar Partida dificultad 3";
            gotoxy(26, 20); cout << "4. Puntajes";
            gotoxy(26, 21); cout << "5. Salir";

            setColor(10);
            gotoxy(23, 23);
            cout << "A/D para mover nave - Número para seleccionar";
            
            // Dibujar nave
            setColor(10);
            gotoxy(nave.getX(), nave.getY());
            cout << "A";
            
            setColor(7);
            
            lastNaveX = nave.getX();
            needRedraw = false;
        }

        if (kbhit()){
            int tecla = getchLinux();
            switch (tecla){
            case 'a': case 'A': 
                nave.moverIzquierda(); 
                needRedraw = true; // Marcar para redibujar
                break;
            case 'd': case 'D': 
                nave.moverDerecha(pantalla.getAncho()); 
                needRedraw = true; // Marcar para redibujar
                break;
            case '1': return 1;
            case '2': return 2;
            case '3': return 3;
            case '4': return 4;
            case '5': return 5;
            }
        }
        
        // Pausa más larga ya que no necesitamos redibujar constantemente
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


// ---------------- MAIN ----------------
int main(){
    hideCursor();
    showSplashScreen();

    while (true){
        int opcion = showMenu();
        if (opcion == 1){
            gameScreen();
        }
        else if (opcion == 2){
            gameScreen();
        }
        else if (opcion == 3){
            gameScreen();
        }
        else if (opcion == 4){
            showScoresScreen();
        }
        else if (opcion == 6){
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
}
