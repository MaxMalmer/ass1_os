# Makefile for Assignment 1 task 1 at the Operating Systems course UMU 22
# Max Malmer, maxmal@cs.umu.se
# Version 1.0, 2022-02-02
CC = gcc
CFLAGS = -std=gnu11 -pthread -lm -g -Wall -Wextra -Werror -Wmissing-declarations -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code

all:			prime_factor

prime_factor: 		
	$(CC) $(CFLAGS) -o prime_factor prime_factor.c

task2: 		
	$(CC) $(CFLAGS) -o prime_factor prime_factor.c

run_prime:			prime_factor
	sudo -S ./prime_factor -p 500 -s f

run_fifo:			prime_factor
	sudo ./prime_factor -p 500 -s f

run_robin:			prime_factor
	sudo ./prime_factor -p 500 -s r

run_manual:			prime_factor
	sudo ./prime_factor -p 500 -s m

valgrind: 		prime_factor
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./prime_factor -p 1000 -s f

helgrind:		prime_factor
	valgrind --tool=helgrind ./prime_factor -p 1000 -s f

clean:
	rm -f prime_factor *.o core

clean_junkfiles:
	rm -f *junkfile*