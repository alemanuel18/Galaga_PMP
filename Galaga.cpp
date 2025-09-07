#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;

// Funciones para manipulación de consola en Linux/Unix
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

void setColor(int color) {
    // Códigos ANSI para colores
    switch(color) {
        case 7:  printf("\033[0m"); break;      // Normal
        case 10: printf("\033[92m"); break;     // Verde brillante
        case 11: printf("\033[96m"); break;     // Cyan brillante
        case 13: printf("\033[95m"); break;     // Magenta
        case 14: printf("\033[93m"); break;     // Amarillo brillante
        case 15: printf("\033[97m"); break;     // Blanco brillante
        default: printf("\033[0m"); break;     // Normal por defecto
    }
}

void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

void clearScreen() {
    system("clear");
}

// Función para leer tecla sin presionar Enter
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Función para dibujar marco decorativo
void drawFrame() {
    setColor(11); // Cyan brillante
    // Marco superior
    gotoxy(0, 0);
    for(int i = 0; i < 80; i++) cout << "═";
    
    // Marco inferior
    gotoxy(0, 24);
    for(int i = 0; i < 80; i++) cout << "═";
    
    // Marco lateral
    for(int i = 1; i < 24; i++) {
        gotoxy(0, i);
        cout << "║";
        gotoxy(79, i);
        cout << "║";
    }
    
    // Esquinas
    gotoxy(0, 0); cout << "╔";
    gotoxy(79, 0); cout << "╗";
    gotoxy(0, 24); cout << "╚";
    gotoxy(79, 24); cout << "╝";
}



// Pantalla de inicio
void showSplashScreen() {
    clearScreen();
    drawFrame();
    
    // Título principal GALAGA en ASCII Art
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
    
    // Texto de continuar
    setColor(10); // Verde brillante
    gotoxy(20, 18);
    cout << "Presiona cualquier tecla para continuar...";
    
    setColor(7); // Restaurar color
    getch(); // Esperar entrada del usuario
}

// Función para mostrar iconos de control
void drawControlIcons() {
    setColor(13); // Magenta
    
    // Icono mover izquierda
    gotoxy(5, 8);
    cout << "┌─────────────┐";
    gotoxy(5, 9);
    cout << "│  ← Mover ←  │";
    gotoxy(5, 10);
    cout << "│      A      │";
    gotoxy(5, 11);
    cout << "└─────────────┘";
    
    
    // Icono mover derecha
    gotoxy(5, 13);
    cout << "┌─────────────┐";
    gotoxy(5, 14);
    cout << "│  → Mover →  │";
    gotoxy(5, 15);
    cout << "│      D      │";
    gotoxy(5, 16);
    cout << "└─────────────┘";
    
    // Icono disparar
    gotoxy(59, 8);
    cout << "┌──────────────┐";
    gotoxy(59, 9);
    cout << "│ ↑ DISPARAR ↑ │";
    gotoxy(59, 10);
    cout << "│   [______]   │";
    gotoxy(59, 11);
    cout << "└──────────────┘";
                                
    // Nave del jugador
    setColor(11);
    gotoxy(60, 13);
    cout << "     ▲";
    gotoxy(60, 14);
    cout << "    ╱A╲";
    gotoxy(60, 15);
    cout << "   ╱───╲";
}

// Función para mostrar el objetivo
void showObjective() {
    setColor(14); // Amarillo
    gotoxy(27, 10);
    cout << "┌────────────────────────┐";
    gotoxy(27, 11);
    cout << "│        OBJETIVO        │";
    gotoxy(27, 12);
    cout << "│                        │";
    gotoxy(27, 13);
    cout << "│  Destruye  todas  las  │";
    gotoxy(27, 14);
    cout << "│  naves alienígenas en  │";
    gotoxy(27, 15);
    cout << "│  todas  las  oleadas   │";
    gotoxy(27, 16);
    cout << "│  para  ganar el juego  │";
    gotoxy(27, 17);
    cout << "└────────────────────────┘";
}

// Función para mostrar menú principal
void showMainMenu() {
    setColor(15); // Blanco brillante
    gotoxy(30, 19);
    cout << "1. Iniciar Partida";
    gotoxy(30, 20);
    cout << "2. Puntajes";
    gotoxy(30, 21);
    cout << "3. Salir";
}

// Función para mostrar menú de modos de juego
void showGameModes() {
    setColor(15); // Blanco brillante
    gotoxy(20, 19);
    cout << "1. Modo 1: 40 alienígenas en 5 grupos de 8";
    gotoxy(20, 20);
    cout << "2. Modo 2: 50 alienígenas en 5 grupos de 10";
    gotoxy(20, 21);
    cout << "3. Modo 3: 60 alienígenas en 6 grupos de 10";
    gotoxy(20, 22);
    cout << "4. Regresar";
}

// Pantalla principal del menú
void showMenuScreen(bool showModes = false) {
    clearScreen();
    drawFrame();
    
    // Título centrado
    setColor(11); // Cyan brillante
    gotoxy(34, 3);
    cout << "G A L A G A";
    
    // Línea decorativa bajo el título
    setColor(14);
    gotoxy(29, 4);
    cout << "━━━━━━━━━━━━━━━━━━━━━";

    // Mostrar controles
    drawControlIcons();
    
    // Mostrar objetivo
    showObjective();
    
    // Mostrar menú correspondiente
    if(!showModes) {
        showMainMenu();
    } else {
        showGameModes();
    }
    
    setColor(7); // Restaurar color
}

// Función principal del programa
int main() {
    hideCursor();
    
    // Mostrar pantalla de inicio
    showSplashScreen();
    
    int option = 0;
    bool inGameModes = false;
    
    while(true) {
        showMenuScreen(inGameModes);
        
        setColor(10); // Verde
        gotoxy(27, 23);
        cout << "Selecciona una opción: ";
        setColor(7);
        
        char choice = getch();
        
        if(!inGameModes) {
            // Menú principal
            switch(choice) {
                case '1':
                    inGameModes = true;
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
                default:
                    // Opción inválida, continuar en el bucle
                    break;
            }
        } else {
            // Menú de modos de juego
            switch(choice) {
                case '1':
                    clearScreen();
                    gotoxy(25, 12);
                    setColor(11);
                    cout << "Iniciando Modo 1: 40 alienígenas...";
                    setColor(10);
                    gotoxy(30, 14);
                    cout << "¡Prepárate para la batalla!";
                    gotoxy(25, 16);
                    cout << "Presiona cualquier tecla para continuar...";
                    setColor(7);
                    getch();
                    // Aquí iría la lógica del juego
                    break;
                case '2':
                    clearScreen();
                    gotoxy(25, 12);
                    setColor(11);
                    cout << "Iniciando Modo 2: 50 alienígenas...";
                    setColor(10);
                    gotoxy(30, 14);
                    cout << "¡Prepárate para la batalla!";
                    gotoxy(25, 16);
                    cout << "Presiona cualquier tecla para continuar...";
                    setColor(7);
                    getch();
                    // Aquí iría la lógica del juego
                    break;
                case '3':
                    clearScreen();
                    gotoxy(25, 12);
                    setColor(11);
                    cout << "Iniciando Modo 3: 60 alienígenas...";
                    setColor(10);
                    gotoxy(30, 14);
                    cout << "¡Prepárate para la batalla!";
                    gotoxy(25, 16);
                    cout << "Presiona cualquier tecla para continuar...";
                    setColor(7);
                    getch();
                    // Aquí iría la lógica del juego
                    break;
                case '4':
                    inGameModes = false;
                    break;
                default:
                    // Opción inválida, continuar en el bucle
                    break;
            }
        }
    }
    
    showCursor();
    return 0;
}
