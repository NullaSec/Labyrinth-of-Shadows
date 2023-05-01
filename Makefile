CC = gcc
CFLAGS = -lncurses -Wall -Wextra -pedantic -O2
SOURCES = ./src/*.c

all: LabyrinthOfShadows run clean

LabyrinthOfShadows:
			$(CC) $(SOURCES) $(CFLAGS) -o LabyrinthOfShadows

run:
			./LabyrinthOfShadows

clean:
			rm LabyrinthOfShadows
