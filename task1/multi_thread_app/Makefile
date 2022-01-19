# Makefile for Laboration 4 5DV088 HT19
# Max Malmer, c17mmr@cs.umu.se
# Version 1.0, 2019-10-17
CC = gcc
CFLAGS = -std=gnu11 -pthread -g -Wall -Wextra -Werror -Wmissing-declarations -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code

all:			mfind

mfind:			mfind.o queue.o list.o
	$(CC) $(CFLAGS) list.o queue.o mfind.o -o mfind

mfind.o:		mfind.c mfind.h queue.h list.h
	$(CC) $(CFLAGS) -c mfind.c

queue.o: 		queue.c queue.h list.h
	$(CC) $(CFLAGS) -c queue.c	

list.o: 		list.c list.h
	$(CC) $(CFLAGS) -c list.c

run:			mfind
	./mfind -p 2 /pkg/comsol/ /usr/local/bin/ comsol

valgrind: 		mfind
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./mfind -p 2 /pkg/comsol/ /usr/local/bin/ comsol

helgrind:		mfind
	valgrind --tool=helgrind ./mfind -p 2 /pkg/comsol/ /usr/local/bin/ comsol

clean:
	rm -f mfind *.o core
