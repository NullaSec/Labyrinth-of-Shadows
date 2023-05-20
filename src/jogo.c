#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Para que serve isto??
#include <ncurses.h>
#include <math.h> // Incluído para a função sqrt

#define LarguraMapa 71
#define AlturaMapa 50
#define MaxQuartos 140
#define TamanhoMinQuarto 12
#define TamanhoMaxQuarto 25
#define MaxMonstros 5
#define VisaoMonstro 6

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

char mapa[AlturaMapa][LarguraMapa];
Jogador jogador;
Quarto quartos[MaxQuartos];
Monstro monstros[MaxMonstros];
int numQuartos = 0;
int numMonstros = 0;
int numMortes = 0;

void iniciarMapa() {
    for (int y = 0; y < AlturaMapa; y++) {
        for (int x = 0; x < LarguraMapa; x++) {
            mapa[y][x] = ' ';
        }
    }
}

void imprimirMapa() {
    for (int y = 0; y < AlturaMapa; y++) {
        for (int x = 0; x < LarguraMapa; x++) {
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
    iniciarMapa();

    srand(time(NULL));

    int i, larguraQuarto, alturaQuarto, x, y;
    for (i = 0; i < MaxQuartos; i++) {
        larguraQuarto = rand() % (TamanhoMaxQuarto - TamanhoMinQuarto + 1) + TamanhoMinQuarto;
        alturaQuarto = rand() % (TamanhoMaxQuarto - TamanhoMinQuarto + 1) + TamanhoMinQuarto;
        x = rand() % (LarguraMapa - larguraQuarto - 1) + 1;
        y = rand() % (AlturaMapa - alturaQuarto - 1) + 1;

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

            int xQuartoAnterior, yQuartoAnterior;
            if (numQuartos > 0) {
                xQuartoAnterior = quartos[numQuartos - 1].x;
                yQuartoAnterior = quartos[numQuartos - 1].y;

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

    for (int i = 0; i < MaxMonstros; i++) {
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

int posicaoValida(int x, int y) {
    if (x < 0 || x >= LarguraMapa || y < 0 || y >= AlturaMapa) {
        return 0; // Fora dos limites do mapa
    }

    if (mapa[y][x] == '.' || mapa[y][x] == '#') {
        return 1; // Posição válida em um quarto ou caverna
    }

    return 0; // Não é uma posição válida
}

void colocarMonstros() {
    numMonstros = 0;
    memset(monstros, 0, sizeof(monstros));
    for (int i = 0; i < numQuartos; i++) {
        int numMonstrosNoQuarto = rand() % 3 + 1;

        int x, y;
        for (int j = 0; j < numMonstrosNoQuarto; j++) {
            x = rand() % (quartos[i].largura - 2) + quartos[i].x + 1;
            y = rand() % (quartos[i].altura - 2) + quartos[i].y + 1;

            if (posicaoValida(x, y)) adicionarMonstro(x, y);
        }
    }
}

void moverJogador(int dx, int dy) {
    
    int novaX, novaY;
    novaX = jogador.x + dx;
    novaY = jogador.y + dy;

    if (novaX >= 0 && novaX < LarguraMapa && novaY >= 0 && novaY < AlturaMapa && mapa[novaY][novaX] == '.') {
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
    int i, dx, dy, novaX, novaY;
    for (i = 0; i < numMonstros; i++) {
        dx = jogador.x - monstros[i].x;
        dy = jogador.y - monstros[i].y;
        double distancia = sqrt(dx * dx + dy * dy);

        if (distancia <= VisaoMonstro) {
            dx = (int)(round(dx / distancia));
            dy = (int)(round(dy / distancia));

            novaX = monstros[i].x + dx;
            novaY = monstros[i].y + dy;

            if (novaX >= 0 && novaX < LarguraMapa && novaY >= 0 && novaY < AlturaMapa && mapa[novaY][novaX] == '.') {
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
            case '8':
            case KEY_UP: moverJogador(0, -1); break;
            case '2':
            case KEY_DOWN: moverJogador(0, 1); break;
            case '4':
            case KEY_LEFT: moverJogador(-1, 0); break;
            case '6':
            case KEY_RIGHT: moverJogador(1, 0); break;
            case KEY_A1: 
            case '7': moverJogador(-1, -1); break;
            case KEY_A3: 
            case '9': moverJogador(1, -1); break;
            case KEY_C1: 
            case '1': moverJogador(-1, 1); break;
            case KEY_C3: 
            case '3': moverJogador(1, +1); break;

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

            default: break;
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
