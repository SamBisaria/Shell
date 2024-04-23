CC=gcc
CFLAGS=-c -Wall -Werror -g
SO_FLAGS=-fPIC -shared

all: main

main: shell.o main.o
	$(CC) shell.o main.o -o main

shell.o: shell.c shell.h
	$(CC) $(CFLAGS) $(SO_FLAGS) shell.c

main.o: main.c shell.h
	$(CC) $(CFLAGS) main.c

clean:
	/bin/rm -f main *.o *.gz

run:
	./main

for_autograder: shell.o
	$(CC) $(SO_FLAGS) -o shell.so shell.o
