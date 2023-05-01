#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "mapa.h" // Inclui a barreira() do mapa.c

typedef struct posi {int posX, posY;} Posicao;

void Movimento(Posicao *pos, int newX, int newY)
{
    pos -> posX += newX;
    pos -> posY += newY;
}

void AlterarMov(Posicao *pos) // Teclas
{
    int tecla = getch();

    switch(tecla)
    {
        case '4':
        case KEY_LEFT: Movimento(pos, -1, 0); break;
        case '6':
        case KEY_RIGHT: Movimento(pos, +1, 0); break;
        case '8':
        case KEY_UP: Movimento(pos, 0, -1); break;
        case '2':
        case KEY_DOWN: Movimento(pos, 0, +1); break;
        case KEY_A1: 
        case '7': Movimento(pos, -1, -1); break;
        case KEY_A3: 
        case '9': Movimento(pos, +1, -1); break;
        case KEY_C1: 
        case '1': Movimento(pos, -1, +1); break;
        case KEY_C3: 
        case '3': Movimento(pos, +1, +1); break;

        case 'q': endwin(); exit(0); break;
        default: break;
    }
}

int main()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);

    Posicao pos = {15,15};

    //box(stdscr, ACS_VLINE, ACS_HLINE); // testar como funciona a caixa
    refresh();

    mvaddch(pos.posY, pos.posX, '@');
    refresh();

    while(1){
        mvprintw(LINES - 1, 0, "Coordenadas: (%d, %d)", pos.posX, pos.posY);
        barreira(); // Testar como é que aparecia a barreira
        //TODO: barreira impenetrável
        AlterarMov(&pos);
        /*
        if (pos.posX < 1) pos.posX = 1; // testar limites
        if (pos.posY < 1) pos.posY = 1;
        if (pos.posX > 70) pos.posX = 70;
        if (pos.posY > 25) pos.posY = 25;
        */
        clear();
        //box(stdscr, ACS_VLINE, ACS_HLINE);
        mvaddch(pos.posY, pos.posX, '@');
        refresh();
    }

    endwin();
    return 0;
}
