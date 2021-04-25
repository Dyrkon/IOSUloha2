// proj2.h
// Řešení IOS-projekt2, 24.4.2021
// Autor: Matěj Mudra, FIT
// Přeloženo: gcc 10.2.0
// hlavičkový soubor k implementaci druhého projektu předmětu IOS

#ifndef IOS_PROJ2_PROJ2_H
#define IOS_PROJ2_PROJ2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

#define MAP_SIZE 1024

#define PRINTERR(E) fprintf(stderr, E)

#define PRINTOUT(F, X) fprintf(F,X);

/*
 * @brief Načte argumenty pokud jsou validní
 * @param argv prohledávané argumenty
 * @param NE ukazatel na počet stkřítků
 * @param NR ukazatel na počet sobů
 * @param TE ukazatel na maximální čas v milisekundách, po kterou skřítek pracuje samostatně.
 * @param TR ukazatel na maximální čas v milisekundách, o které se sob vrací z dovolené domů.
 * @return po úspěšném načtení vrací 0 jinak 1
 */
int load_args(char **argv, int *NE, int *NR, int *TE, int *TR);

/*
 * @brief Načte potřební porměné pro běh porgramu
 * @param parament počet argumentů, argumenty, výpisový soubor, všechy počítadla
 * @return po úspěšném otevření vrací 0 jinak 1
 */
int setup(int argc, char **argv, int *NE, int *NR, int *TE, int *TR);

/*
 * @brief Připravý sdílenou paměť
 * @param mem_size velikost paměti
 * @return vrací adresu paměti
 */
void* prep_memory(size_t mem_size);

#endif //IOS_PROJ2_PROJ2_H
