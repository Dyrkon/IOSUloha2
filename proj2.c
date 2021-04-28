// proj2.c
// Řešení IOS-projekt2, 24.4.2021
// Autor: Matěj Mudra, FIT
// Přeloženo: gcc 10.2.0
// Implementace druhého projektu předmětu IOS

#include "proj2.h"

int main(int argc, char **argv) {
    args_t args;
    void *shem = NULL;
    sem_t *sems[N_SEMAPHORES];

    // Načtení vstupních parametrů
    if(setup(argc, argv, &args) == 1)
        exit(1);

    if ((shem = prep_memory(sizeof(personnel_t))) == MAP_FAILED) {
        PRINTERR("Sdílenou paměť se nezdažilo namapovat\n");
        return 1;
    }

    if (prep_sems(sems)){
        PRINTERR("Semafory se nezdažilo namapovat\n");
        return 1;
    }

    run_proj(&args, shem, sems);

    close_mem(sizeof(personnel_t), shem);
    close_sems(sems, N_SEMAPHORES);
    fclose(args.file);

    return 0;
}

int run_proj(args_t *args, personnel_t *personnel, sem_t *sems[])
{
    personnel_t init = {0, 0, 0,
                        0, 0, 0,
                        0, false, true};
    *personnel = init;
    int elf_id = 0;
    int deer_id = 0;

    for(int i = 0; i < (args->NE + args->NR + 1); ++i)
    {
        switch (fork()) {
            // Dítě vytvořeno
            case 0:
                // První proces je Santa
                if (i == 0) {
                    santa(args, personnel, sems);
                }
                // Pokud je ještě třeba, tak se přidá elf
                LOC_SEM(MUTEX);
                if (personnel->active_elves < args->NE) {
                    personnel->active_elves++;
                    elf_id = personnel->active_elves;
                    UNLOC_SEM(MUTEX);
                    elf(elf_id, args, personnel, sems);
                }
                UNLOC_SEM(MUTEX);
                // Pokud je ještě třeba, tak se přidá sob
                LOC_SEM(MUTEX);
                if (personnel->active_reindeers < args->NR) {
                    personnel->active_reindeers++;
                    deer_id = personnel->active_reindeers;
                    UNLOC_SEM(MUTEX);
                    deer(deer_id, args, personnel, sems);
                }
                UNLOC_SEM(MUTEX);
                break;
            case -1:
                PRINTERR("Naskytla se chyba pri forkovani procesu\n");
                return 1;
            default:
                break;
        }
    }
    LOC_SEM(END);

    return 0;
}

void santa(args_t *args, personnel_t *personnel, sem_t *sems[])
{
    PRIN_FLUSHT(stdout, "%d: Santa: going to sleep\n", ++(personnel->action_counter));

    while (true)
    {
        LOC_SEM(SANTA);
        LOC_SEM(MUTEX);
        if(personnel->reindeers_back == args->NR)
        {
            PRIN_FLUSHT(stdout, "%d: Santa: closing workshop\n", ++(personnel->action_counter));
            for(int i = 0; i < personnel->active_reindeers; ++i)
                UNLOC_SEM(REINDEER);
            for (int i = 0; i < 3; ++i)
                UNLOC_SEM(ELF);
            personnel->workshop_empty = false;
            personnel->christmas_closed = true;
            UNLOC_SEM(MUTEX);
            LOC_SEM(ALL_HITHCED);
            PRIN_FLUSHT(stdout, "%d: Santa: Christmas started\n", ++(personnel->action_counter));
            LOC_SEM(ALL_DONE);
            UNLOC_SEM(END);
            exit(0);
        }
        else if(personnel->elves_in_line == 3)
        {
            PRIN_FLUSHT(stdout, "%d: Santa: helping elves\n", ++(personnel->action_counter));
            for (int i = 0; i < 3; ++i)
                UNLOC_SEM(ELF);
            personnel->elves_in_line -= 3;
            personnel->workshop_empty = true;
        }
        UNLOC_SEM(MUTEX);
    }

}

// TODO write to file instead of stdout
void elf(int elfID, args_t *args, personnel_t *personnel, sem_t *sems[]) {
    LOC_SEM(MUTEX);
    PRIN_FLUSHT(stdout, "%d: Elf %d: started\n", ++(personnel->action_counter), elfID);
    UNLOC_SEM(MUTEX);

    while (true)
    {
        // Skřítek pracuje
        SLEEP_MILS(0, args->TE);

        LOC_SEM(MUTEX);
        PRIN_FLUSHT(stdout, "%d: Elf %d: need help\n", ++(personnel->action_counter), elfID);
        if (personnel->christmas_closed) {
            PRIN_FLUSHT(stdout, "%d: Elf %d: taking holidays\n", ++(personnel->action_counter), elfID);
            UNLOC_SEM(MUTEX);
            exit(0);
        }
        UNLOC_SEM(MUTEX);

        LOC_SEM(MUTEX);
        personnel->elves_in_line++;
        if (personnel->workshop_empty == true &&
            personnel->christmas_closed == false &&
            personnel->elves_in_line == 3) {
            UNLOC_SEM(MUTEX);
            UNLOC_SEM(SANTA);
        } else
            UNLOC_SEM(MUTEX);

        LOC_SEM(ELF);
        LOC_SEM(MUTEX);
        if (!personnel->christmas_closed)
            PRIN_FLUSHT(stdout, "%d: Elf %d: get help\n", ++(personnel->action_counter), elfID);
        UNLOC_SEM(MUTEX);

        if (personnel->christmas_closed)
        {
            LOC_SEM(MUTEX);
            PRIN_FLUSHT(stdout, "%d: Elf %d: taking holidays\n", ++(personnel->action_counter), elfID);
            personnel->elves_on_vacation++;
            if (personnel->elves_on_vacation == personnel->active_elves)
                UNLOC_SEM(ALL_DONE);
            UNLOC_SEM(MUTEX);
            exit(0);
        }
    }
}

/*
 * 1.Každý sob je identifikován číslem rdID, 0<rdID<=NR
 * 2.Po spuštění vypíše: A: RD rdID: rstarted
 * 3.Čas na dovolené modelujte voláním usleep na náhodný interval <TR/2,TR>
 * 4.Po návratu z letní dovolené vypíše: A: RD rdID: return home a následně čeká, než ho Santa zapřáhne k saním.
 *   Pokud je posledním sobem, který se vrátil z dovolené, tak vzbudí Santu.
 * 5.Po zapřažení do saní vypíše: A: RD rdID: get hitched a následně proces končí
*/
void deer(int rdID, args_t *args, personnel_t *personnel, sem_t *sems[])
{
    // Zamknu si semafor se zápisem, pošlu soba na dovolenou, zápis odemknu
    LOC_SEM(MUTEX);
    PRIN_FLUSHT(stdout, "%d: RD %d: rstarted\n", ++(personnel->action_counter), rdID)
    UNLOC_SEM(MUTEX);

    // Dovolená
    SLEEP_MILS(args->TR/2, args->TR);

    // Sob se vrátí z dovolené
    LOC_SEM(MUTEX);
    PRIN_FLUSHT(stdout, "%d: RD %d: return\n", ++(personnel->action_counter), rdID);
    personnel->reindeers_back++;
    if (personnel->reindeers_back == personnel->active_reindeers)
    {
        UNLOC_SEM(MUTEX);
        UNLOC_SEM(SANTA);
    }
    else
        UNLOC_SEM(MUTEX);

    LOC_SEM(REINDEER);
    LOC_SEM(MUTEX);
    PRIN_FLUSHT(stdout, "%d: RD %d: get hitched\n", ++(personnel->action_counter), rdID);
    personnel->hitched_reindeers++;
    if (personnel->hitched_reindeers == personnel->active_reindeers)
        UNLOC_SEM(ALL_HITHCED);
    UNLOC_SEM(MUTEX);

    exit(0);
}

int prep_sems(sem_t *semaphs[])
{
    for(int i = 0; i < N_SEMAPHORES; ++i)
    {
        if ((semaphs[i] = prep_memory(sizeof(sem_t))) == MAP_FAILED)
        {
            for(int j = 0; j < i; j++)
            {
                if (j != 0) {
                    if((sem_destroy(semaphs[j-1])) == -1)
                        PRINTERR("Nepodarilo se uvolnit semafor\n");
                }
                close_mem(sizeof(sem_t), semaphs[j]);
            }
            return 1;
        }
        if((sem_init(semaphs[i], 1, (i == MUTEX ? 1 : 0))) == -1)
        {
            for(int j = 0; j < i; j++)
            {
                if((sem_destroy(semaphs[j])) == -1)
                    PRINTERR("Nepodarilo se uvolnit semafor\n");
                close_mem(sizeof(sem_t), semaphs[j]);
            }
            return 1;
        }
    }

    return 0;
}

int close_sems(sem_t *semaphs[], int Nsems) {
    int result = 0;
    for (int i = 0; i < Nsems; ++i)
    {
        if((sem_destroy(semaphs[i])) == -1)
            PRINTERR("Nepodarilo se uvolnit semafor\n");
        close_mem(sizeof(sem_t), semaphs[i]);
        if (result != 1)
            result = 1;
    }
    return result;
}

void* prep_memory(size_t mem_size) {
    int access = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    return mmap(NULL, mem_size, access, visibility, 0, 0);
}

void close_mem(size_t size, void *pointer)
{
    munmap(pointer, size);
}

int load_args(char **argv, args_t *args)
{
    int temp = 0;
    for(int i = 1; argv[i]; ++i)
    {
        if((temp = (int)strtol(argv[i], NULL, 10)) <= 0)
            return 1;
        else
        {
            switch (i) {
                case 1:
                    if(temp >= 1000)
                        return 1;
                    args->NE = temp;
                    break;

                case 2:
                    if(temp >= 20)
                        return 1;
                    args->NR = temp;
                    break;

                case 3:
                    if(temp >= 1000)
                        return 1;
                    args->TE = temp;
                    break;

                case 4:
                    if(temp >= 1000)
                        return 1;
                    args->TR = temp;
                    break;
                default:
                    return 1;
            }
        }
    }
    return 0;
}

int open_file(args_t *args)
{
    if((args->file = fopen("proj2.txt","w")) == NULL)
    {
        PRINTERR("Soubor pro vypis se nepodarilo otevrit\n");
        return 1;
    }
    return 0;
}

int setup(int argc, char **argv, args_t *args)
{
    if (argc != 5)
    {
        PRINTERR("Nespravny pocet parametru\n");
        return 1;
    }

    if (load_args(argv, args))
    {
        PRINTERR("Nacitani argumentu selhalo\n");
        return 1;
    }

    if (open_file(args))
    {
        PRINTERR("Otevreni souboru se nezdarilo\n");
        return 1;
    }

    return 0;
}

int get_rand(int floor, int roof)
{
    srand(time(NULL));
    return ((rand() % (roof - floor)) + floor);
}
