CC = gcc
CFLAGS = -Wall -ansi -pedantic -g -lm

all: fw

fw: fw.c hashmap.c
	$(CC) $(CFLAGS) -o fw fw.c hashmap.c

clean:
	rm -f *.o
