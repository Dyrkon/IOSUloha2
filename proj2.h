// proj2.h
// Řešení IOS-projekt2, 24.4.2021
// Autor: Matěj Mudra, FIT
// Přeloženo: gcc 10.2.0
// hlavičkový soubor k implementaci druhého projektu předmětu IOS

#ifndef IOS_PROJ2_PROJ2_H
#define IOS_PROJ2_PROJ2_H

#include <stdio.h>
#include <stdlib.h>
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
#include <time.h>
#include <stdbool.h>

// Počet semaforů
#define N_SEMAPHORES 7

// Vypiš chybu
#define PRINTERR(E) fprintf(stderr, E)

// Vypíše do souboru
#define PRIN_FLUSHT(F, ...) {     \
        fprintf(F, __VA_ARGS__);  \
        fflush(NULL);             \
}

#define TEST {\
    printf("ELF %d\n", *((int **)sems)[ELF]); \
    printf("SANTA %d\n", *((int **)sems)[SANTA]); \
    printf("REINDEER %d\n", *((int **)sems)[REINDEER]); \
    printf("MUTEX %d\n", *((int **)sems)[MUTEX]); \
    printf("ALL_DONE %d\n", *((int **)sems)[ALL_DONE]); \
    printf("END %d\n", *((int **)sems)[END]); \
    printf("ALL_HITHCED %d\n", *((int **)sems)[ALL_HITHCED]); \
    fflush(NULL);                 \
}

// Makro pro zamknutí semaforu
#define LOC_SEM(X) sem_wait(sems[X])

// Makro pro odemknutí semaforu
#define UNLOC_SEM(X) sem_post(sems[X])

#define SLEEP_MILS(F,R) (usleep(get_rand(F, R) * 1000))

// Seznam semaforů
enum semaphores_e{SANTA, ELF, REINDEER, MUTEX, END, ALL_HITHCED, ALL_DONE};

// Strukt s argumenty a soubory
typedef struct args
{
    int NE;
    int NR;
    int TE;
    int TR;
    FILE *file;
}args_t;

// Strukt se stavem Santova personálu
typedef struct personnel
{
    int action_counter;
    int active_elves;
    int active_reindeers;
    int reindeers_back;
    int hitched_reindeers;
    int elves_on_vacation;
    int elves_in_line;
    bool christmas_closed;
    bool workshop_empty;
}personnel_t;


/*
 * @brief Načte argumenty pokud jsou validní
 * @param argv prohledávané argumenty
 * @param args struct s argumenty a výposovým souborem
 * @return po úspěšném načtení vrací 0 jinak 1
 */
int load_args(char **argv, args_t *args);

/*
 * @brief Načte potřební porměné pro běh porgramu
 * @param params argumenty, výpisový soubor, všechy počítadla
 * @return po úspěšném nastavení vrací 0 jinak 1
 */
int setup(int argc, char **argv, args_t *args);

/*
 * @brief Připravý sdílenou paměť
 * @param mem_size velikost paměti
 * @return vrací adresu paměti
 */
void* prep_memory(size_t mem_size);

/*
 * @brief Uvolní sdílenou mapu
 * @param size velikost paměti
 * @param pointer ukazatel na paměť
 */
void close_mem(size_t size, void *pointer);

/*
 * @brief Namapuje do paměti semafory
 * @param semaphs pole ukazatelů na semafory
 * @param Nsems počet semaforů
 * @return po úspěšném namapování vrací 0 jinak 1
 */
int prep_sems(sem_t *semaphs[]);

/*
 * @brief Vymaže semafory z paměti
 * @param semaphs pole ukazatelů na semafory
 * @param Nsems počet semaforů
 * @return po úspěšném vymazání vrací 0 jinak 1
 */
int close_sems(sem_t *semaphs[], int Nsems);

/*
 * @brief Otevře soubor pro výpis
 * @param args struct s argumenty a výposovým souborem
 * @retun po úspěšném otevření vrací 0 jinak 1
 */
int open_file(args_t *args);

/*
 * @brief Spustí projekt
 * @param args vstupní argumenty
 * @param shem odkaz na sdílenou paměť
 * @param sems semafory
 * @retun po úspěšném otevření vrací 0 jinak 1
 */
int run_proj(args_t *args, personnel_t *personnel, sem_t *sems[]);

/*
 * @brief Vrátí náhodné číslo menší než strop
 * @param roof maximální hodnota
 * @param floor minimální hodnota
 * @retun náhodné číslo
 */
int get_rand(int floor, int roof);

/*
 * @brief Proces sob
 * @param rdID id soba
 * @param args argumenty
 * @param shem ukazatel do sdílené paměti
 * @param sems semafory
 */
void deer(int rdID, args_t *args, personnel_t *personnel, sem_t *sems[]);

/*
 * @brief Proces elf
 * @param elfID id elfa
 * @param args argumenty
 * @param shem ukazatel do sdílené paměti
 * @param sems semafory
 */
void elf(int elfID, args_t *args, personnel_t *personnel, sem_t *sems[]);

/*
 * @brief Proces santa
 * @param elfID id elfa
 * @param args argumenty
 * @param shem ukazatel do sdílené paměti
 * @param sems semafory
 */
void santa(args_t *args, personnel_t *personnel, sem_t *sems[]);

#endif //IOS_PROJ2_PROJ2_H
