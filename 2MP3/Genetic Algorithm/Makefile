CC = gcc
CFLAGS = -Wall -Wextra -lm -pg


all: GA

GA: GA.o OF.o functions.o 
	$(CC) $(CFLAGS) GA.o OF.o functions.o -o GA -lm

OF.o: OF.c
	$(CC) $(CFLAGS) -c OF.c -lm 

functions.o: functions.c
	$(CC) $(CFLAGS) -c functions.c 

GA.o: GA.c
	$(CC) $(CFLAGS) -c GA.c

clean: 
	rm -f GA *.o
