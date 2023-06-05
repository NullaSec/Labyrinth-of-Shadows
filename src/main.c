#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <ncurses.h>
#include <math.h> // Incluído para a função sqrt

#define LarguraMapa 72
#define AlturaMapa 50
#define MaxQuartos 150
#define TamanhoMinQuarto 12
#define TamanhoMaxQuarto 25
#define MaxMonstros 30
#define VisaoMonstro 8

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
int numMonstrosMortos = 0;

void iniciarMapa() {
    for (int y = 0; y < AlturaMapa; y++) {
        for (int x = 0; x < LarguraMapa; x++) {
            mapa[y][x] = ' ';
        }
    }
    // Preencher as paredes horizontais 
    for (int x = 0; x < LarguraMapa; x++) {
        mapa[0][x] = '#';
        mapa[AlturaMapa - 1][x] = '#';
    }

// Preencher as paredes verticais
    for (int y = 0; y < AlturaMapa; y++) {
        mapa[y][0] = '#';
        mapa[y][LarguraMapa - 1] = '#';
    }   

}

int verificarVisibilidade(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    double xIncrement = dx / (double)steps;
    double yIncrement = dy / (double)steps;
    double x = x1;
    double y = y1;

    for (int i = 0; i < steps; i++) {
        x += xIncrement;
        y += yIncrement;

        int mapX = (int)x;
        int mapY = (int)y;

        if (mapa[mapY][mapX] == '#') {
            return 0; // Raio encontrou uma parede, posição não é visível
        }
    }

    return 1; // Raio atingiu a posição sem encontrar paredes, posição é visível
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
                        if (verificarVisibilidade(jogador.x, jogador.y, x, y)) { // Verifica a visibilidade
                            printw("%c ", mapa[y][x]);
                        } else {
                            printw("# "); // Exibe uma parede se estiver fora da visibilidade
                        }
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
    printw("Número de Monstros mortos: %d / 5\n", numMonstrosMortos);
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
    int salaAdjacente = 0;
    for (int x = x1; x <= x2; x++) {
        if (mapa[y][x] == '.')
            salaAdjacente = 1;
        mapa[y][x] = '.';
        mapa[y+1][x] = '.';
    }
    if (!salaAdjacente) {
        int x = (x1 + x2) / 2;
        mapa[y][x] = '.';
        mapa[y+1][x] = '.';
    }
}

void criarTunelVertical(int y1, int y2, int x) {
    int salaAdjacente = 0;
    for (int y = y1; y <= y2; y++) {
        if (mapa[y][x] == '.')
            salaAdjacente = 1;
        mapa[y][x] = '.';
        mapa[y][x+1] = '.';
    }
    if (!salaAdjacente) {
        int y = (y1 + y2) / 2;
        mapa[y][x] = '.';
        mapa[y][x+1] = '.';
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
            if (numQuartos > 0) {
                int quartoAnterior = rand() % numQuartos;
                int xQuartoAnterior = quartos[quartoAnterior].x + quartos[quartoAnterior].largura / 2;
                int yQuartoAnterior = quartos[quartoAnterior].y + quartos[quartoAnterior].altura / 2;

                int xNovoQuarto = novoQuarto.x + novoQuarto.largura / 2;
                int yNovoQuarto = novoQuarto.y + novoQuarto.altura / 2;

                if (rand() % 2 == 0) {
                   criarTunelHorizontal(xQuartoAnterior, xNovoQuarto, yQuartoAnterior);
                   criarTunelVertical(yQuartoAnterior, yNovoQuarto, xNovoQuarto);
                } else {
                   criarTunelVertical(yQuartoAnterior, yNovoQuarto, xQuartoAnterior);
                   criarTunelHorizontal(xQuartoAnterior, xNovoQuarto, yNovoQuarto);
                }
            }

            numQuartos++;
        }
    }

// Preencher as paredes horizontais
    for (int x = 0; x < LarguraMapa; x++) {
        mapa[0][x] = '#';
        mapa[AlturaMapa - 1][x] = '#';
    }

// Preencher as paredes verticais
    for (int y = 0; y < AlturaMapa; y++) {
        mapa[y][0] = '#';
        mapa[y][LarguraMapa - 1] = '#';
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

void preencherVazios() {
    for (int y = 0; y < AlturaMapa; y++) {
        for (int x = 0; x < LarguraMapa; x++) {
            if (mapa[y][x] == ' ') {
                mapa[y][x] = '#';
            }
        }
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

    for (int i = 0; i < numMonstros; i++) {
        if (x == monstros[i].x && y == monstros[i].y) {
            return 0; // Há um monstro nessa posição
        }
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
            do {
                x = rand() % (quartos[i].largura - 2) + quartos[i].x + 1;
                y = rand() % (quartos[i].altura - 2) + quartos[i].y + 1;
            } while (!posicaoValida(x, y)); // Verifica se a posição gerada é válida

            adicionarMonstro(x, y);

            if (numMonstros >= MaxMonstros) { // Adiciona apenas o número máximo de monstros ao mapa
                return;
            }
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

int main() {
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    gerarMapa();
    colocarMonstros();
    preencherVazios();

    while (1) {
        clear();
        imprimirMapa();

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
                            numMonstrosMortos++;
                        }
                    }
                }
                break;

            default: break;
        }

        if (numMonstrosMortos == 5) {
            clear();
            printw("Parabéns! Você derrotou todos os monstros!\n");
            refresh();
            getch();
            endwin();
            return 0;
        }

        moverMonstros();

        if (numMortes > 0) {
            clear();
            printw("Você foi morto por um monstro!\n");
            printw("Número de monstros mortos: %d\n", numMonstrosMortos);
            refresh();
            getch();
            endwin();
            return 0;
        }
    }
}