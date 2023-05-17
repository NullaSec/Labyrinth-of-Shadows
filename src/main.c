#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define MAP_WIDTH 51
#define MAP_HEIGHT 100
#define MAX_ROOMS 100
#define ROOM_MIN_SIZE 3
#define ROOM_MAX_SIZE 6

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

char map[MAP_HEIGHT][MAP_WIDTH];
Player player;
Room rooms[MAX_ROOMS];
int numRooms = 0;

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
                printw("%c ", map[y][x]);
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

int main() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    noecho();

    generateMap();

    player.x = rooms[0].x + rooms[0].width / 2;
    player.y = rooms[0].y + rooms[0].height / 2;

    while (1) {
        clear();
        printMap();

        int input = getch();

        switch (input) {
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
    }

    endwin();
    return 0;
}
