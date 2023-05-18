#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <math.h> // Incluído para a função sqrt

#define MAP_WIDTH 71
#define MAP_HEIGHT 50
#define MAX_ROOMS 140
#define ROOM_MIN_SIZE 6
#define ROOM_MAX_SIZE 14
#define MAX_MONSTERS 5
#define MONSTER_SIGHT_RANGE 6

typedef struct {
    int x;
    int y;
} Player;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Room;

typedef struct {
    int x;
    int y;
} Monster;

char map[MAP_HEIGHT][MAP_WIDTH];
Player player;
Room rooms[MAX_ROOMS];
Monster monsters[MAX_MONSTERS];
int numRooms = 0;
int numMonsters = 0;
int numDeaths = 0;

void initializeMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = '#';
        }
    }
}

void printMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == player.x && y == player.y) {
                printw("@ ");
            } else if (abs(x - player.x) <= 5 && abs(y - player.y) <= 5) {
                int isMonster = 0;
                for (int i = 0; i < numMonsters; i++) {
                    if (x == monsters[i].x && y == monsters[i].y) {
                        isMonster = 1;
                        break;
                    }
                }
                if (isMonster) {
                    printw("M ");
                } else {
                    printw("%c ", map[y][x]);
                }
            } else {
                printw("  "); // Espaço em branco para áreas fora do campo de visão
            }
        }
        printw("\n");
    }
    refresh();
}

void createRoom(Room room) {
    for (int y = room.y; y < room.y + room.height; y++) {
        for (int x = room.x; x < room.x + room.width; x++) {
            map[y][x] = '.';
        }
    }
}

void createHorizontalTunnel(int x1, int x2, int y) {
    for (int x = x1; x <= x2; x++) {
        map[y][x] = '.';
    }
}

void createVerticalTunnel(int y1, int y2, int x) {
    for (int y = y1; y <= y2; y++) {
        map[y][x] = '.';
    }
}

void generateMap() {
    initializeMap();

    srand(time(NULL));

    for (int i = 0; i < MAX_ROOMS; i++) {
        int roomWidth = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int roomHeight = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int x = rand() % (MAP_WIDTH - roomWidth - 1) + 1;
        int y = rand() % (MAP_HEIGHT - roomHeight - 1) + 1;

        Room newRoom = { x, y, roomWidth, roomHeight };
        int failed = 0;

        for (int j = 0; j < numRooms; j++) {
            if (newRoom.x <= rooms[j].x + rooms[j].width &&
                newRoom.x + newRoom.width >= rooms[j].x &&
                newRoom.y <= rooms[j].y + rooms[j].height &&
                newRoom.y + newRoom.height >= rooms[j].y) {
                failed = 1;
                break;
            }
        }

        if (!failed) {
            createRoom(newRoom);

            if (numRooms > 0) {
                int prevX = rooms[numRooms - 1].x + rooms[numRooms - 1].width / 2;
                int prevY = rooms[numRooms - 1].y + rooms[numRooms - 1].height / 2;
                int currX = newRoom.x + newRoom.width / 2;
                int currY = newRoom.y + newRoom.height / 2;

                if (rand() % 2 == 0) {
                    createHorizontalTunnel(prevX, currX, prevY);
                    createVerticalTunnel(prevY, currY, currX);
                } else {
                    createVerticalTunnel(prevY, currY, prevX);
                    createHorizontalTunnel(prevX, currX, currY);
                }
            }

            rooms[numRooms++] = newRoom;
        }
    }

    player.x = rooms[0].x + rooms[0].width / 2;
    player.y = rooms[0].y + rooms[0].height / 2;
}

void movePlayer(int dx, int dy) {
    int newX = player.x + dx;
    int newY = player.y + dy;

    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == '.') {
        player.x = newX;
        player.y = newY;
    }
}

void moveMonsters() {
    for (int i = 0; i < numMonsters; i++) {
        int dx = player.x - monsters[i].x;
        int dy = player.y - monsters[i].y;
        int distance = sqrt(dx * dx + dy * dy);

        if (distance <= MONSTER_SIGHT_RANGE) {
            if (dx != 0) {
                dx /= distance;
            }
            if (dy != 0) {
                dy /= distance;
            }

            int newX = monsters[i].x + dx;
            int newY = monsters[i].y + dy;

            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == '.') {
                monsters[i].x = newX;
                monsters[i].y = newY;

                if (newX == player.x && newY == player.y) {
                    numDeaths++;
                    returnToMenu();
                }
            }
        }
    }
}

void initializeGame() {
    clear();
    generateMap();
    numMonsters = rand() % MAX_MONSTERS + 1;

    for (int i = 0; i < numMonsters; i++) {
        int x, y;

        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.' || (x == player.x && y == player.y));

        monsters[i].x = x;
        monsters[i].y = y;
    }

    // Atualiza a posição inicial do jogador
    do {
        player.x = rand() % MAP_WIDTH;
        player.y = rand() % MAP_HEIGHT;
    } while (map[player.y][player.x] != '.');
}


void returnToMenu() {
    clear();
    printw("Você morreu!\n");
    printw("Número de mortes: %d\n", numDeaths);
    printw("Pressione qualquer tecla para voltar a jogar...\n");
    refresh();
    getch();
    initializeGame();
}

void menu() {
    clear();
    printw("Bem-vindo ao Jogo!\n");
    printw("Pressione qualquer tecla para começar...\n");
    refresh();
    getch();
    initializeGame();
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    menu();

    while (1) {
        clear();
        printMap();

        int key = getch();

        switch (key) {
            case KEY_UP:
                movePlayer(0, -1);
                break;
            case KEY_DOWN:
                movePlayer(0, 1);
                break;
            case KEY_LEFT:
                movePlayer(-1, 0);
                break;
            case KEY_RIGHT:
                movePlayer(1, 0);
                break;
            case 'q':
                endwin();
                return 0;
        }

        moveMonsters();
    }

    return 0;
}
