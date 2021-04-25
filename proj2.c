// proj2.c
// Řešení IOS-projekt2, 24.4.2021
// Autor: Matěj Mudra, FIT
// Přeloženo: gcc 10.2.0
// Implementace druhého projektu předmětu IOS

#include "proj2.h"

int main(int argc, char **argv){

    int NE, NR, TE, TR = 0;
    int action_counter = 1;
    FILE *file = NULL;
    void *shem = NULL;

    // Načtení vstupních parametrů
    if(setup(argc, argv, &NE, &NR, &TE, &TR) == 1)
        exit(1);

    // Otevření souboru pro výpis
    if((file = fopen("proj2.txt","w")) == NULL)
    {
        PRINTERR("Soubor pro vypis se nepodarilo otevrit\n");
        exit(1);
    }

    // TODO
    if ((shem = prep_memory(MAP_SIZE)) == MAP_FAILED)
    {
        PRINTERR("Sdílenou paměť se nezdažilo namapovat\n");
        fclose(file);
    }

    memcpy(shem, "shared\n", 8);
    memcpy(shem, "shared\n", 8);

    int pid = fork();
    if(pid == 0)
    {
        char dest[10];
        memcpy(dest, shem,8);
        printf("Ja jsem dite\n");
        printf("1 %s\n", dest);
        memcpy(shem, "shared++\n", 10);
        exit(1);
    }
    else
    {
        usleep(500);
        printf("Ja jsem rodic\n");
    }

    char fin[10];
    memcpy(fin, shem,10);
    printf("2 %s\n", fin);

    munmap(shem, MAP_SIZE);

    fclose(file);

    return 0;
}


void* prep_memory(size_t mem_size) {
    int access = PROT_READ | PROT_WRITE;

    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    return mmap(NULL, mem_size, access, visibility, -1, 0);
}

int load_args(char **argv, int *NE, int *NR, int *TE, int *TR)
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
                    *NE = temp;
                    break;

                case 2:
                    if(temp >= 20)
                        return 1;
                    *NR = temp;
                    break;

                case 3:
                    if(temp >= 1000)
                        return 1;
                    *TE = temp;
                    break;

                case 4:
                    if(temp >= 1000)
                        return 1;
                    *TR = temp;
                    break;
                default:
                    return 1;
            }
        }
    }

    return 0;
}

int setup(int argc, char **argv, int *NE, int *NR, int *TE, int *TR)
{
    if (argc != 5)
    {
        PRINTERR("Nespravny pocet parametru\n");
        return 1;
    }

    if (load_args(argv, NE, NR, TE, TR))
    {
        PRINTERR("Nacitani argumentu selhalo\n");
        return 1;
    }

    return 0;
}
