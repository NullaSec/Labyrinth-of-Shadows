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
            } else {
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

        Room newRoom = {x, y, roomWidth, roomHeight};

        int failed = 0;

        for (int j = 0; j < numRooms; j++) {
            if (newRoom.x < rooms[j].x + rooms[j].width &&
                newRoom.x + newRoom.width > rooms[j].x &&
                newRoom.y < rooms[j].y + rooms[j].height &&
                newRoom.y + newRoom.height > rooms[j].y) {
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

                if (rand() % 2) {
                    createHorizontalTunnel(prevX, currX, prevY);
                    createVerticalTunnel(prevY, currY, currX);
                } else {
                    createVerticalTunnel(prevY, currY, prevX);
                    createHorizontalTunnel(prevX, currX, currY);
                }
            }

            rooms[numRooms] = newRoom;
            numRooms++;
        }
    }
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
        // Verificar a distância entre o monstro e o jogador
        int distanceX = player.x - monsters[i].x;
        int distanceY = player.y - monsters[i].y;

        // Verificar se o monstro está a uma distância menor ou igual a 6 do jogador
        if (abs(distanceX) <= MONSTER_SIGHT_RANGE && abs(distanceY) <= MONSTER_SIGHT_RANGE) {
            // Movimentar o monstro em direção ao jogador
            int dx = 0;
            int dy = 0;

            if (distanceX > 0) {
                dx = 1;
            } else if (distanceX < 0) {
                dx = -1;
            }

            if (distanceY > 0) {
                dy = 1;
            } else if (distanceY < 0) {
                dy = -1;
            }

            int newX = monsters[i].x + dx;
            int newY = monsters[i].y + dy;

            // Verificar se o movimento é válido
            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == '.') {
                monsters[i].x = newX;
                monsters[i].y = newY;
            }
        } else {
            // Movimentar o monstro aleatoriamente
            int randomDirection = rand() % 4; // 0: cima, 1: baixo, 2: esquerda, 3: direita
            int dx = 0;
            int dy = 0;

            switch (randomDirection) {
                case 0:
                    dy = -1;
                    break;
                case 1:
                    dy = 1;
                    break;
                case 2:
                    dx = -1;
                    break;
                case 3:
                    dx = 1;
                    break;
            }

            int newX = monsters[i].x + dx;
            int newY = monsters[i].y + dy;

            // Verificar se o movimento é válido
            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == '.') {
                monsters[i].x = newX;
                monsters[i].y = newY;
            }
        }

        // Verificar se o monstro se encontrou com o jogador
        if (monsters[i].x == player.x && monsters[i].y == player.y) {
            clear();
            printw("Você morreu!\n");
            refresh();
            getch();
            endwin();
            exit(0);
        }
    }
}

int main() {
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);

    generateMap();

    player.x = rooms[0].x + rooms[0].width / 2;
    player.y = rooms[0].y + rooms[0].height / 2;

    numMonsters = 0;

    for (int i = 0; i < MAX_MONSTERS; i++) {
        int roomIndex = rand() % numRooms;
        int x = rooms[roomIndex].x + rand() % rooms[roomIndex].width;
        int y = rooms[roomIndex].y + rand() % rooms[roomIndex].height;

        monsters[i].x = x;
        monsters[i].y = y;

        numMonsters++;
    }

    while (1) {
        clear();
        printMap();
        refresh();

        int ch = getch();

        switch (ch) {
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

    endwin();
    return 0;
}