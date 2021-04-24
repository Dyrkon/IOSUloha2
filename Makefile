# Makefile
# Řešení IOS-projekt2, 24.4.2021
# Autor: Matěj Mudra, FIT
# Přeloženo: gcc 10.2.0
# Makefile pro přeložení druhého projektu IOS


CFLAGS  	= -g -std=c99 -Wextra -Wall -pedantic -O2 -march=native -flto
DEBUGFLAGS	= -Og -g -rdynamic -std=c11 -Wextra -Wall -pedantic
CC 			= gcc

.PHONY: clean

all: main

main.o: main.c
	$(CC) $(DEBUGFLAGS) -c main.c

main: main.o
	$(CC) $(DEBUGFLAGS) main.o -o main


clean:
	rm *.o main