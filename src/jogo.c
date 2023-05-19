#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <math.h> // Incluído para a função sqrt

#define MAP_WIDTH 71
#define MAP_HEIGHT 50
#define MAX_ROOMS 140
#define ROOM_MIN_SIZE 12
#define ROOM_MAX_SIZE 25
#define MAX_MONSTERS 5
#define MONSTER_SIGHT_RANGE 6

typedef struct {
    int x;
    int y;
    int saude;
    char weapon[20]; // Adicionada a variável para a arma atual do jogador
} Jogador;

typedef struct {
    int x;
    int y;
    int largura;
    int altura;
} Quarto;

typedef struct {
    int x;
    int y;
    int saude;
} Monstro;

char mapa[MAP_HEIGHT][MAP_WIDTH];
Jogador jogador;
Quarto quartos[MAX_ROOMS];
Monstro monstros[MAX_MONSTERS];
int numQuartos = 0;
int numMonstros = 0;
int numMortes = 0;

void inicializarMapa() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            mapa[y][x] = ' ';
        }
    }
}

void imprimirMapa() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int dx = x - jogador.x;
            int dy = y - jogador.y;
            double distancia = sqrt(dx * dx + dy * dy);

            if (distancia <= 7) { // Alterado o campo de visão para um círculo com raio 7
                if (x == jogador.x && y == jogador.y) {
                    printw("@ ");
                } else {
                    int ehMonstro = 0;
                    for (int i = 0; i < numMonstros; i++) {
                        if (x == monstros[i].x && y == monstros[i].y) {
                            ehMonstro = 1;
                            break;
                        }
                    }
                    if (ehMonstro) {
                        printw("M ");
                    } else {
                        printw("%c ", mapa[y][x]);
                    }
                }
            } else {
                printw("  "); // Espaço em branco para áreas fora do campo de visão
            }
        }
        printw("\n");
    }
    printw("Vida do jogador: %d\n", jogador.saude);
    printw("Arma atual: %s\n", jogador.weapon); // Exibe a arma atual do jogador
    refresh();
}

void criarQuarto(Quarto quarto) {
    for (int y = quarto.y + 1; y < quarto.y + quarto.altura - 1; y++) {
       for (int x = quarto.x + 1; x < quarto.x + quarto.largura - 1; x++) {
        mapa[y][x] = '.';
       }
    } 
}

void criarTunelHorizontal(int x1, int x2, int y) {
    for (int x = x1 + 1; x <= x2 - 1; x++) {
        mapa[y][x] = '.';
    }
}

void criarTunelVertical(int y1, int y2, int x) {
    for (int y = y1 + 1; y <= y2 - 1; y++) {
        mapa[y][x] = '.';
    }
}

void gerarMapa() {
    inicializarMapa();

    srand(time(NULL));

    for (int i = 0; i < MAX_ROOMS; i++) {
        int larguraQuarto = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int alturaQuarto = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int x = rand() % (MAP_WIDTH - larguraQuarto - 1) + 1;
        int y = rand() % (MAP_HEIGHT - alturaQuarto - 1) + 1;

        Quarto novoQuarto = { x, y, larguraQuarto, alturaQuarto };

        int falhou = 0;
        for (int j = 0; j < numQuartos; j++) {
            if (novoQuarto.x < quartos[j].x + quartos[j].largura &&
                novoQuarto.x + novoQuarto.largura > quartos[j].x &&
                novoQuarto.y < quartos[j].y + quartos[j].altura &&
                novoQuarto.y + novoQuarto.altura > quartos[j].y) {
                falhou = 1;
                break;
            }
        }

        if (!falhou) {
            criarQuarto(novoQuarto);

            if (numQuartos > 0) {
                int xQuartoAnterior = quartos[numQuartos - 1].x;
                int yQuartoAnterior = quartos[numQuartos - 1].y;

                if (rand() % 2 == 0) {
                    criarTunelHorizontal(xQuartoAnterior, novoQuarto.x + novoQuarto.largura / 2, yQuartoAnterior + quartos[numQuartos - 1].altura / 2);
                    criarTunelVertical(yQuartoAnterior, novoQuarto.y + novoQuarto.altura / 2, novoQuarto.x + novoQuarto.largura / 2);
                } else {
                    criarTunelVertical(yQuartoAnterior, novoQuarto.y + novoQuarto.altura / 2, xQuartoAnterior + quartos[numQuartos - 1].largura / 2);
                    criarTunelHorizontal(xQuartoAnterior, novoQuarto.x + novoQuarto.largura / 2, novoQuarto.y + novoQuarto.altura / 2);
                }
            }

            quartos[numQuartos] = novoQuarto;
            numQuartos++;
        }
    }

    jogador.x = quartos[0].x + quartos[0].largura / 2;
    jogador.y = quartos[0].y + quartos[0].altura / 2;
    jogador.saude = 100;
    strcpy(jogador.weapon, "Faca"); // Define a arma inicial do jogador como "Faca"

    for (int i = 0; i < MAX_MONSTERS; i++) {
        monstros[i].x = -1;
        monstros[i].y = -1;
        monstros[i].saude = 0;
    }
}

void adicionarMonstro(int x, int y) {
    Monstro novoMonstro = { x, y, 20 };
    monstros[numMonstros] = novoMonstro;
    numMonstros++;
}

void colocarMonstros() {
    numMonstros = 0;
    memset(monstros, 0, sizeof(monstros));
    for (int i = 0; i < numQuartos; i++) {
        int numMonstrosNoQuarto = rand() % 3 + 1;

        for (int j = 0; j < numMonstrosNoQuarto; j++) {
            int x = rand() % (quartos[i].largura - 2) + quartos[i].x + 1;
            int y = rand() % (quartos[i].altura - 2) + quartos[i].y + 1;

           if (posicaoValida(x, y)) {
            adicionarMonstro(x, y);
        }
           }
    }
}

int posicaoValida(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return 0; // Fora dos limites do mapa
    }

    if (mapa[y][x] == '.' || mapa[y][x] == '#') {
        return 1; // Posição válida em um quarto ou caverna
    }

    return 0; // Não é uma posição válida
}


void moverJogador(int dx, int dy) {
    int novaX = jogador.x + dx;
    int novaY = jogador.y + dy;

    if (novaX >= 0 && novaX < MAP_WIDTH && novaY >= 0 && novaY < MAP_HEIGHT && mapa[novaY][novaX] == '.') {
        jogador.x = novaX;
        jogador.y = novaY;

        for (int i = 0; i < numMonstros; i++) {
            if (monstros[i].x == jogador.x && monstros[i].y == jogador.y) {
                jogador.saude -= 10; // Reduz a vida do jogador ao ser atacado pelo monstro
                if (jogador.saude <= 0) {
                    numMortes++;
                    return;
                }
            }
        }
    }
}

void moverMonstros() {
    for (int i = 0; i < numMonstros; i++) {
        int dx = jogador.x - monstros[i].x;
        int dy = jogador.y - monstros[i].y;
        double distancia = sqrt(dx * dx + dy * dy);

        if (distancia <= MONSTER_SIGHT_RANGE) {
            dx = (int)(round(dx / distancia));
            dy = (int)(round(dy / distancia));

            int novaX = monstros[i].x + dx;
            int novaY = monstros[i].y + dy;

            if (novaX >= 0 && novaX < MAP_WIDTH && novaY >= 0 && novaY < MAP_HEIGHT && mapa[novaY][novaX] == '.') {
                // Verifica se a nova posição coincide com a posição atual do jogador
                if (novaX != jogador.x || novaY != jogador.y) {
                    monstros[i].x = novaX;
                    monstros[i].y = novaY;
                }

                if (monstros[i].x == jogador.x && monstros[i].y == jogador.y) {
                    jogador.saude -= 10;
                    if (jogador.saude <= 0) {
                        numMortes++;
                        return;
                    }
                }
            }
        }
    }
}

void imprimirArmaJogador() {
    printw("Arma atual: %s\n", jogador.weapon);
}

int main() {
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    gerarMapa();
    colocarMonstros();

    while (1) {
        clear();
        imprimirMapa();
        imprimirArmaJogador(); // Adiciona a função para exibir a arma atual do jogador

        int key = getch();

        switch (key) {
            case KEY_UP:
                moverJogador(0, -1);
                break;
            case KEY_DOWN:
                moverJogador(0, 1);
                break;
            case KEY_LEFT:
                moverJogador(-1, 0);
                break;
            case KEY_RIGHT:
                moverJogador(1, 0);
                break;
            case 'q':
                endwin();
                return 0;
            case 'e':
                for (int i = 0; i < numMonstros; i++) {
                    if (abs(monstros[i].x - jogador.x) <= 1 && abs(monstros[i].y - jogador.y) <= 1) {
                        monstros[i].saude -= 20; // Reduz a saúde do monstro ao atacá-lo
                        if (monstros[i].saude <= 0) {
                            for (int j = i; j < numMonstros - 1; j++) {
                                monstros[j] = monstros[j + 1];
                            }
                            numMonstros--;
                        }
                    }
                }
                break;
            default:
                break;
        }

        if (numMonstros == 0) {
            clear();
            printw("Parabéns! Você derrotou todos os monstros!\n");
            printw("Número de mortes: %d\n", numMortes);
            refresh();
            getch();
            endwin();
            return 0;
        }

        moverMonstros();

        if (numMortes > 0) {
            clear();
            printw("Você foi morto por um monstro!\n");
            printw("Número de mortes: %d\n", numMortes);
            refresh();
            getch();
            endwin();
            return 0;
        }
    }
}
