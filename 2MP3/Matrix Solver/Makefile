CC = gcc
CFLAGS = -Wall -Wextra -lm -pg -fprofile-arcs -ftest-coverage

all: main

main: main.o functions.o 
	$(CC) $(CFLAGS) main.o functions.o -o main -lm

functions.o: functions.c
	$(CC) $(CFLAGS) -c functions.c -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -lm

clean: 
	rm -f main *.o
