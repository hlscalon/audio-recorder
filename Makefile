CC=gcc
FILES=src/*.c
OPTS=-Wall
LIBS=-lpulse -lpulse-simple -lsox

main:
	$(CC) $(FILES) $(OPTS) $(LIBS) -o audio-recorder
