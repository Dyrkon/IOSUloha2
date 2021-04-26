// proj2.c
// Řešení IOS-projekt2, 24.4.2021
// Autor: Matěj Mudra, FIT
// Přeloženo: gcc 10.2.0
// Implementace druhého projektu předmětu IOS

#include "proj2.h"

int main(int argc, char **argv) {
    int action_counter = 1;
    args_t args;
    sem_t semaphores[3];

    // Načtení vstupních parametrů
    if(setup(argc, argv, &args) == 1)
        exit(1);

    run_proj(&args);

    fclose(args.file);

    return 0;
}

int run_proj(args_t *args)
{
    personnel_t personnel;
    personnel.active_elves = 0;
    personnel.active_reindeers = 0;
    void *shem = NULL;


    if ((shem = prep_memory(sizeof(personnel_t))) == MAP_FAILED)
    {
        PRINTERR("Sdílenou paměť se nezdažilo namapovat\n");
        return 1;
    }

    for(int i = 0; (args->NE + args->NR) >= i; ++i)
    {
        switch (fork()) {
            // Dítě vytvořeno
            case 0:
                // První proces je Santa
                if (i == 0)
                {
                    santa();
                    return 0;
                }
                // Pokud je ještě třeba, tak se přidá elf
                if (((personnel_t*)shem)->active_elves < args->NE)
                {
                    if (personnel.active_elves < args->NE)
                    {
                        ((personnel_t*)shem)->active_elves++;
                        elf(((personnel_t*)shem)->active_elves-1);
                    }
                    return 0;
                }
                // Pokud je ještě třeba, tak se přidá sob
                if (((personnel_t*)shem)->active_reindeers < args->NR)
                {
                    if (personnel.active_reindeers < args->NR)
                    {
                        ((personnel_t*)shem)->active_reindeers++;
                        deer(((personnel_t*)shem)->active_reindeers-1);
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

void elf(int elf_id)
{
    // TODO
    printf("N %d elf created\n", elf_id);
}

void deer(int deer_id)
{
    // TODO
    printf("N %d raindeer created\n", deer_id);
}

void close_mem(size_t size, void *pointer)
{
    munmap(pointer, size);
}

void* prep_memory(size_t mem_size) {
    int access = PROT_READ | PROT_WRITE;

    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    return mmap(NULL, mem_size, access, visibility, -1, 0);
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

int get_rand(int roof)
{
    int rand_num = 0;
    srand(time(NULL));
    rand_num = rand();
    rand_num %= roof;
    return rand_num > 0 ? rand_num : rand_num*(-1);
}
