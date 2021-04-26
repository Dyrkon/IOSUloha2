# Makefile
# Řešení IOS-projekt2, 24.4.2021
# Autor: Matěj Mudra, FIT
# Přeloženo: gcc 10.2.0
# Makefile pro přeložení druhého projektu IOS


CFLAGS  	= -g -std=gnu99 -Wextra -Wall -pedantic -O2 -march=native -flto
DEBUGFLAGS	= -Og -g -rdynamic -std=gnu99 -Wextra -Wall -pedantic
ADDITIONAL	= -pthread
CC 			= gcc

.PHONY: clean

all: proj2

proj2.o: proj2.c proj2.h
	$(CC) $(DEBUGFLAGS) -c proj2.c $(ADDITIONAL)

proj2: proj2.o
	$(CC) $(DEBUGFLAGS) proj2.o -o proj2 $(ADDITIONAL)


clean:
	rm -f *.o *.out *.txt proj2