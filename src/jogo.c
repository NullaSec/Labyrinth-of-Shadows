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
    int health;
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
    int health;
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
    printw("Vida do jogador: %d\n", player.health);
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
                int prevRoomX = rooms[numRooms - 1].x;
                int prevRoomY = rooms[numRooms - 1].y;

                if (rand() % 2 == 0) {
                    createHorizontalTunnel(prevRoomX, newRoom.x + newRoom.width / 2, prevRoomY + rooms[numRooms - 1].height / 2);
                    createVerticalTunnel(prevRoomY, newRoom.y + newRoom.height / 2, newRoom.x + newRoom.width / 2);
                } else {
                    createVerticalTunnel(prevRoomY, newRoom.y + newRoom.height / 2, prevRoomX + rooms[numRooms - 1].width / 2);
                    createHorizontalTunnel(prevRoomX, newRoom.x + newRoom.width / 2, newRoom.y + newRoom.height / 2);
                }
            }

            rooms[numRooms] = newRoom;
            numRooms++;
        }
    }

    player.x = rooms[0].x + rooms[0].width / 2;
    player.y = rooms[0].y + rooms[0].height / 2;
    player.health = 100;

    for (int i = 0; i < MAX_MONSTERS; i++) {
        monsters[i].x = -1;
        monsters[i].y = -1;
        monsters[i].health = 0;
    }
}

void addMonster(int x, int y) {
    Monster newMonster = { x, y, 20 };
    monsters[numMonsters] = newMonster;
    numMonsters++;
}

void placeMonsters() {
    for (int i = 0; i < numRooms; i++) {
        int numMonstersInRoom = rand() % 3 + 1;

        for (int j = 0; j < numMonstersInRoom; j++) {
            int x = rand() % (rooms[i].width - 2) + rooms[i].x + 1;
            int y = rand() % (rooms[i].height - 2) + rooms[i].y + 1;

            addMonster(x, y);
        }
    }
}

void movePlayer(int dx, int dy) {
    int newX = player.x + dx;
    int newY = player.y + dy;

    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == '.') {
        player.x = newX;
        player.y = newY;

        for (int i = 0; i < numMonsters; i++) {
            if (monsters[i].x == player.x && monsters[i].y == player.y) {
                player.health -= 10; // Reduz a vida do jogador ao ser atacado pelo monstro
                if (player.health <= 0) {
                    numDeaths++;
                    return;
                }
            }
        }
    }
}

void moveMonsters() {
    for (int i = 0; i < numMonsters; i++) {
        int dx = player.x - monsters[i].x;
        int dy = player.y - monsters[i].y;
        double distance = sqrt(dx * dx + dy * dy);

        if (distance <= MONSTER_SIGHT_RANGE) {
            dx = (int)(round(dx / distance));
            dy = (int)(round(dy / distance));

            int newX = monsters[i].x + dx;
            int newY = monsters[i].y + dy;

            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == '.') {
                monsters[i].x = newX;
                monsters[i].y = newY;

                if (monsters[i].x == player.x && monsters[i].y == player.y) {
                    player.health -= 10; // Reduz a vida do jogador ao ser atacado pelo monstro
                    if (player.health <= 0) {
                        numDeaths++;
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

    generateMap();
    placeMonsters();

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
            case 'e':
                for (int i = 0; i < numMonsters; i++) {
                    if (abs(monsters[i].x - player.x) <= 1 && abs(monsters[i].y - player.y) <= 1) {
                        monsters[i].health -= 20; // Dano da faca
                        if (monsters[i].health <= 0) {
                            // Monstro morto
                            monsters[i] = monsters[numMonsters - 1];
                            numMonsters--;
                        }
                        break;
                    }
                }
                break;
            default:
                break;
        }

        moveMonsters();

        if (numDeaths > 0) {
            clear();
            printw("Game Over! Você morreu %d vez(es).\n", numDeaths);
            refresh();
            getch();
            endwin();
            return 0;
        }
    }

    endwin();
    return 0;
}
