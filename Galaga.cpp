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

string getPlayerName(bool isVictory = false)
{
    clearScreen();
    drawFrame();

    if (isVictory) setColor(10);      // Verde brillante para victoria
    else setColor(14);                // Amarillo para game over / derrota

    gotoxy(30, 10);
    if (isVictory) cout << "¡VICTORIA!";
    else cout << "¡GAME OVER!";

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
    //Variables globales de la partida jugandose
    bool gameRunning = true;
    int score = 0;
    int naveX = 40;
    int naveY = 20;

    //Posiciones de los disparos de la anave
    vector<pair<int, int>> disparos;

    //Posicionees, velocidades y movimientos de las entidades
    vector<Enemigo> enemigos;
    vector<int> direccionesX;
    vector<int> velocidadHorizontal;
    vector<int> velocidadVertical;
    vector<int> contadoresH;
    vector<int> contadoresV;
    vector<int> tiposMovimiento;

    // Crear enemigos
    for (int i = 0; i < 5; i++) {
        enemigos.emplace_back(i * 12 + 10, 3 + (i % 3));
        direccionesX.push_back((i % 2 == 0) ? 1 : -1);
        velocidadHorizontal.push_back(5 + i); // frames por movimiento horizontal
        velocidadVertical.push_back(15 + i*5); // frames por movimiento vertical
        contadoresH.push_back(0);
        contadoresV.push_back(0);
        tiposMovimiento.push_back(i % 3);
    }

    bool inGame = false;

    // Pantalla inicial
    clearScreen();
    drawFrame();
    setColor(15);
    gotoxy(30, 8); cout << "GALAGA - MODO JUEGO";
    setColor(14);
    gotoxy(15, 12); cout << "ESPACIO - Disparar ";
    gotoxy(15, 14); cout << "A/D - Mover nave";
    gotoxy(15, 16); cout << "M - Terminar partida y guardar puntaje";
    gotoxy(15, 18); cout << "Q - Salir sin guardar";
    setColor(13);
    gotoxy(20, 20); cout << "¡Destruye enemigos antes de que te alcancen!";
    setColor(10);
    gotoxy(20, 22); cout << "Presiona cualquier tecla para comenzar...";
    getchLinux();
    inGame = true;

    while (gameRunning)
    {
        if (inGame) {

            // Mover disparos
            for (int i = disparos.size() - 1; i >= 0; i--) {
                disparos[i].second--;
                if (disparos[i].second < 2)
                    disparos.erase(disparos.begin() + i);
            }

            // Colisiones disparos - enemigos
            for (int i = disparos.size() - 1; i >= 0; i--) {
                for (int j = enemigos.size() - 1; j >= 0; j--) {
                    if (disparos[i].first == enemigos[j].getX() &&
                        disparos[i].second == enemigos[j].getY()) {
                        score += 200;
                        disparos.erase(disparos.begin() + i);
                        enemigos.erase(enemigos.begin() + j);
                        direccionesX.erase(direccionesX.begin() + j);
                        velocidadHorizontal.erase(velocidadHorizontal.begin() + j);
                        velocidadVertical.erase(velocidadVertical.begin() + j);
                        contadoresH.erase(contadoresH.begin() + j);
                        contadoresV.erase(contadoresV.begin() + j);
                        tiposMovimiento.erase(tiposMovimiento.begin() + j);
                        break;
                    }
                }
            }

            // Movimiento de enemigos
            for (int i = 0; i < enemigos.size(); i++) {
                contadoresH[i]++;
                contadoresV[i]++;

                // Movimiento horizontal
                if (contadoresH[i] >= velocidadHorizontal[i]) {
                    contadoresH[i] = 0;
                    if (direccionesX[i] == 1) enemigos[i].moveRight(1);
                    else enemigos[i].moveLeft(1);
                    if (enemigos[i].getX() <= 3 || enemigos[i].getX() >= 75)
                        direccionesX[i] *= -1;
                }

                // Movimiento vertical
                if (contadoresV[i] >= velocidadVertical[i]) {
                    contadoresV[i] = 0;
                    enemigos[i].moveDown(1);
                }
            }

            // Revisar si hay victoria
            if (enemigos.empty()) {
                setColor(10);
                gotoxy(30, 12);
                cout << "¡VICTORIA TOTAL!";
                gotoxy(28, 14);
                cout << "¡Todos los enemigos eliminados!";
                setColor(14);
                gotoxy(32, 16);
                cout << "BONUS: +1000 puntos";
                setColor(7);
                score += 1000;
                sleep(4);
                string playerName = getPlayerName(true);
                highScores.push_back({playerName, score});
                showScoresScreen();
                gameRunning = false;
                continue;
            }

            // Colisión directa con la nave
            bool gameOver = false;
            for (auto &enemigo : enemigos) {
                if (enemigo.getX() == naveX && enemigo.getY() >= naveY - 1) {
                    setColor(12);
                    gotoxy(30, 12); cout << "¡COLISIÓN DIRECTA!";
                    gotoxy(35, 14); cout << "GAME OVER";
                    setColor(7);
                    sleep(3);
                    if (score > 0) {
                        string playerName = getPlayerName();
                        highScores.push_back({playerName, score});
                        showScoresScreen();
                    }
                    gameOver = true;
                    break;
                }
            }

            // Invasión completada
            for (auto &enemigo : enemigos) {
                if (enemigo.getY() >= 22) {
                    setColor(12);
                    gotoxy(28, 12); cout << "¡INVASIÓN COMPLETADA!";
                    gotoxy(35, 14); cout << "GAME OVER";
                    setColor(7);
                    sleep(3);
                    if (score > 0) {
                        string playerName = getPlayerName();
                        highScores.push_back({playerName, score});
                        showScoresScreen();
                    }
                    gameOver = true;
                    break;
                }
            }

            if (gameOver) {
                gameRunning = false;
                continue;
            }

            // Dibujar todo
            clearScreen();
            drawFrame();

            setColor(15);
            gotoxy(25, 1); cout << "GALAGA - PUNTAJE: " << score;
            setColor(14);
            gotoxy(2, 1); cout << "ENEMIGOS: " << enemigos.size();
            setColor(13);
            gotoxy(60, 1); cout << "DISPAROS: " << disparos.size();
            gotoxy(50, 1); cout << "VIDAS: ♥♥♥";

            // Disparos
            setColor(11);
            for (auto &d : disparos) {
                gotoxy(d.first, d.second); cout << "|";
            }

            // Enemigos
            setColor(12);
            for (int i = 0; i < enemigos.size(); i++) {
                gotoxy(enemigos[i].getX(), enemigos[i].getY());
                switch (tiposMovimiento[i]) {
                    case 0: cout << "X"; break;
                    case 1: cout << "▼"; break;
                    case 2: cout << "◆"; break;
                }
            }

            // Nave
            setColor(10);
            gotoxy(naveX, naveY); cout << "A";

            // Instrucciones
            setColor(11);
            gotoxy(2, 22);
            cout << "A/D: Mover - ESPACIO: Disparar - M: Terminar - Q: Salir - P: Pausa";
            setColor(7);

            // Controles
            if (kbhit()) {
                int tecla = getchLinux();
                switch (tecla) {
                    case 'a': case 'A': if (naveX > 2) naveX--; break;
                    case 'd': case 'D': if (naveX < 77) naveX++; break;
                    case ' ': if (disparos.size() < 5) disparos.push_back({naveX, naveY - 1}); break;
                    case 'm': case 'M':
                        if (score > 0) {
                            string playerName = getPlayerName();
                            highScores.push_back({playerName, score});
                            showScoresScreen();
                        }
                        gameRunning = false; break;
                    case 'q': case 'Q': gameRunning = false; break;
                    case 'p': case 'P': inGame = false; break;
                }
            }
        } else {
            // Pausa
            clearScreen();
            drawFrame();
            setColor(15); gotoxy(30, 8); cout << "JUEGO PAUSADO";
            gotoxy(28, 10); cout << "PUNTAJE: " << score;
            setColor(14);
            gotoxy(15, 14); cout << "ESPACIO - Continuar jugando";
            gotoxy(15, 16); cout << "M - Terminar y guardar puntaje";
            gotoxy(15, 18); cout << "Q - Salir sin guardar";
            setColor(13); gotoxy(8, 21);
            cout << "PUNTUACIÓN: Enemigo destruido = +200pts, Victoria = +1000pts";

            if (kbhit()) {
                int tecla = getchLinux();
                switch (tecla) {
                    case ' ': inGame = true; break;
                    case 'm': case 'M':
                        if (score > 0) {
                            string playerName = getPlayerName();
                            highScores.push_back({playerName, score});
                            showScoresScreen();
                        }
                        gameRunning = false; break;
                    case 'q': case 'Q': gameRunning = false; break;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Menú principal
void showMainMenu()
{
    bool menuRunning = true;
    while (menuRunning)
    {
        clearScreen();
        drawFrame();

        setColor(15);
        gotoxy(30, 8);
        cout << "MENU PRINCIPAL GALAGA";

        setColor(10);
        gotoxy(32, 12);
        cout << "1. Iniciar Juego";

        gotoxy(32, 13);
        cout << "2. Instrucciones";

        gotoxy(32, 14);
        cout << "3. Puntajes";

        gotoxy(32, 15);
        cout << "4. Creditos";

        gotoxy(32, 16);
        cout << "5. Salir";

        setColor(14);
        gotoxy(25, 20);
        cout << "Selecciona una opcion (1-5): ";

        setColor(7);
        char opcion = getchLinux();

        switch (opcion)
        {
        case '1':
            gameScreen();
            break;
        case '2':
            clearScreen();
            drawFrame();
            setColor(14);
            gotoxy(34, 5);
            cout << "INSTRUCCIONES";
            drawControlIcons();
            setColor(10);
            gotoxy(22, 22);
            cout << "Presiona cualquier tecla para regresar...";
            getchLinux();
            break;
        case '3':
            showScoresScreen();
            break;
        case '4':
            clearScreen();
            drawFrame();
            setColor(15);
            gotoxy(34, 5);
            cout << "CREDITOS";
            setColor(11);
            gotoxy(28, 10);
            cout << "Desarrollado por: ";
            gotoxy(28,11);
            cout << "Marco Díaz";
            gotoxy(28,12);
            cout << "Marcelo Detlefsen";
            gotoxy(28,13);
            cout << "Alejandro Jerez";
            gotoxy(28,14);
            cout << "Julián Divas";
            gotoxy(28, 15);
            cout << "Curso: Programacion de microprocesadores";
            gotoxy(28, 16);
            cout << "Universidad del Valle de Guatemala";
            setColor(10);
            gotoxy(22, 22);
            cout << "Presiona cualquier tecla para regresar...";
            getchLinux();
            break;
        case '5':
            menuRunning = false;
            break;
        }
    }
}

// ---------------- MAIN ----------------
int main()
{
    hideCursor();
    showSplashScreen();
    showMainMenu();
    showCursor();
    return 0;
}
