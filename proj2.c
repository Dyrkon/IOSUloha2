// proj2.c
// Řešení IOS-projekt2, 24.4.2021
// Autor: Matěj Mudra, FIT
// Přeloženo: gcc 10.2.0
// Implementace druhého projektu předmětu IOS

#include "proj2.h"

int main(int argc, char **argv) {
    int action_counter = 1;
    args_t args;

    // Načtení vstupních parametrů
    if(setup(argc, argv, &args) == 1)
        exit(1);

    run_proj(&args);

    fclose(args.file);

    return 0;
}

int run_proj(args_t *args)
{
    void *shem = NULL;
    sem_t *sems[N_SEMAPHORES];
    personnel_t personnel;
    personnel.active_elves = 0;
    personnel.active_reindeers = 0;
    personnel.reindeers_back = 0;
    personnel.elves_in_line = 0;
    personnel.christmas_closed = false;
    personnel.workshop_empty = true;

    if ((shem = prep_memory(sizeof(personnel_t))) == MAP_FAILED) {
        PRINTERR("Sdílenou paměť se nezdažilo namapovat\n");
        return 1;
    }

    if (prep_sems(sems, N_SEMAPHORES)){
        PRINTERR("Semafory se nezdažilo namapovat\n");
        return 1;
    }

    for(int i = 0; (args->NE + args->NR) >= i; ++i)
    {
        switch (fork()) {
            // Dítě vytvořeno
            case 0:
                // První proces je Santa
                if (i == 0) {
                    santa();
                    return 0;
                }
                // Pokud je ještě třeba, tak se přidá elf
                if (((personnel_t*)shem)->active_elves < args->NE) {
                    if (personnel.active_elves < args->NE) {
                        // LOC_SEM(MUTEX);
                        ((personnel_t*)shem)->active_elves++;
                        // UNLOC_SEM(MUTEX);
                        elf(((personnel_t*)shem)->active_elves, args, shem, sems);
                    }
                    return 0;
                }
                // Pokud je ještě třeba, tak se přidá sob
                if (((personnel_t*)shem)->active_reindeers < args->NR) {
                    if (personnel.active_reindeers < args->NR)
                    {
                        // LOC_SEM(MUTEX);
                        ((personnel_t*)shem)->active_reindeers++;
                        // UNLOC_SEM(MUTEX);
                        deer(((personnel_t*)shem)->active_reindeers, args, shem, sems);
                    }
                    return 0;
                }
                break;
            case -1:
                PRINTERR("Naskytla se chyba pri forkovani procesu\n");
                return 1;
            default:
                break;
        }
    }

    close_mem(sizeof(personnel_t), shem);

    return 0;
}

void santa()
{
    // TODO
    printf("Santa is here baby\n");

}

/*
 * 1.Každý skřítek je unikátně identifikován číslem elfID. 0<elfID<=NE
 * 2.Po spuštění vypíše: A: Elf elfID: started
 * 3.Samostatnou práci modelujte voláním funkce usleep na náhodný čas v intervalu <0,TE>.
 * 4.Když skončí samostatnou práci, potřebuje pomoc od Santy.
 *      Vypíše: A: Elf elfID: need help a zařadí se do fronty před Santovou dílnou.
 * 5.Pokud je třetí ve frontě před dílnou, dílna je prázdná a na dílně není cedule „Vánoce – zavřeno“,
 *      tak společně s prvním a druhým z fronty vstoupí do dílny a vzbudí Santu.
 * 6.Skřítek v dílně dostane pomoc a vypíše: A: Elf elfID: get help  (na pořadí pomoci skřítkům v dílně nezáleží)
 * 7.Po obdržení pomoci ihned odchází z dílny a pokud je dílna již volná,
 *      tak při odchodu z dílny může upozornit čekající skřítky, že už je volno (volitelné).
 * 8.Pokud je na dveřích dílny nápis „Vánoce – zavřeno“ vypíše: A: Elf elfID: taking holidays a proces ihned skončí
 */

void elf(int elfID, args_t *args, void *shem, sem_t *sems[])
{
    // TODO

    PRIN_FLUSHT(stdout, "%d: Elf %d: started\n", ++(((personnel_t *)shem)->action_counter), elfID);

    // Skřítek pracuje
    usleep(get_rand(0, args->TE));

    PRIN_FLUSHT(stdout, "%d: Elf %d: need help\n", ++(((personnel_t *)shem)->action_counter), elfID);
    ((personnel_t *)shem)->elves_in_line++;
    if (((personnel_t *)shem)->workshop_empty &&
        !((personnel_t *)shem)->christmas_closed &&
        ((personnel_t *)shem)->elves_in_line == 3)
    {
        // UNLOC_SEM(SANTA);
        PRIN_FLUSHT(stdout, "%d: Elf %d: get help\n", ++(((personnel_t *)shem)->action_counter), elfID);
    }

    if (((personnel_t *)shem)->christmas_closed)
        PRIN_FLUSHT(stdout, "%d: Elf %d: taking holidays\n", ++(((personnel_t *)shem)->action_counter), elfID);
}

/*
 * 1.Každý sob je identifikován číslem rdID, 0<rdID<=NR
 * 2.Po spuštění vypíše: A: RD rdID: rstarted
 * 3.Čas na dovolené modelujte voláním usleep na náhodný interval <TR/2,TR>
 * 4.Po návratu z letní dovolené vypíše: A: RD rdID: return home a následně čeká, než ho Santa zapřáhne k saním.
 *   Pokud je posledním sobem, který se vrátil z dovolené, tak vzbudí Santu.
 * 5.Po zapřažení do saní vypíše: A: RD rdID: get hitched a následně proces končí
*/
// TODO write to file instead of stdout
void deer(int rdID, args_t *args, void *shem, sem_t *sems[])
{
    // TODO resolve MUTEX

    // Zamknu si semafor se zápisem, pošlu soba na dovolenou, zápis odemknu
    // LOC_SEM(MUTEX);
    PRIN_FLUSHT(stdout, "%d: RD %d: rstarted\n", ++(((personnel_t *)shem)->action_counter), rdID)
    if (((personnel_t *)args)->reindeers_back == 9)
        // UNLOC_SEM(SANTA);
    // UNLOC_SEM(MUTEX);

    // Dovolená
    usleep(get_rand(args->TR/2, args->TR));

    // Sob se vrátí z dovolené
    // LOC_SEM(MUTEX);
    PRIN_FLUSHT(stdout, "%d: RD %d: return\n", ++(((personnel_t *)shem)->action_counter), rdID);
    ((personnel_t *)args)->reindeers_back++;
    // UNLOC_SEM(MUTEX);

    // LOC_SEM(REINDEER);
    PRIN_FLUSHT(stdout, "%d: RD %d: get hitched\n", ++(((personnel_t *)shem)->action_counter), rdID);
}

int prep_sems(sem_t *semaphs[], int Nsems)
{
    for(int i = 0; i < Nsems; ++i)
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
        if((sem_init(semaphs[i], 1, 0)) == -1)
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
    return (rand() % (roof - floor + 1)) + floor;
}
