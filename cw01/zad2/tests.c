#include "../zad1/difflib.h"
#include "../zad1/difflib.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

#define print_time printTime(calTime(rtstart, rtend), calTime(tstart->tms_stime, tend->tms_stime), calTime(tstart->tms_utime, tend->tms_utime))

double calTime(clock_t start, clock_t end)
{
    return ((double) (end - start)) / sysconf(_SC_CLK_TCK);
}

void printTime(double rtime, double stime, double utime)
{
    printf("r: %f, s: %f, u: %f\n", rtime, stime, utime);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("You passed no arguments!\n");
        return 1;
    }
    struct MainArray *mainarr = NULL;
    struct tms *tstart = calloc(1, sizeof(struct tms)), *tend = calloc(1, sizeof(struct tms));
    clock_t rtstart, rtend;
    rtstart = times(tstart);
    int i = 1;
    while (i < argc)
    {
        if (mainarr == NULL)
        {
            if (strcmp(argv[i], "create_array") == 0)
            {
                if (argc <= i + 1)
                {
                    printf("You have to pass the size of the main array!\n");
                    return 1;
                }
                mainarr = createMainArray(atoi(argv[i + 1]));
                i += 2;
            }
            else
            {
                printf("Use create_array before %s at %d!\n", argv[i], i);
                return 1;
            }
        }
        else
        {
            if (strcmp(argv[i], "create_array") == 0)
            {
                printf("Array already created at %d!\n", i);
                return 1;
            }
            else if (strcmp(argv[i], "compare_pairs") == 0)
            {
                i++;
                if (argc <= i)
                {
                    printf("You have to pass the number of file pairs!\n");
                    return 1;
                }
                int pairsNum = atoi(argv[i]);
                if (i + pairsNum * 2 > argc)
                {
                    printf("Not enough files for compare_pairs at %d!\n", i - 1);
                    return 1;
                }
                i++;
                char *str = calloc(pairsNum * 50, sizeof(char));
                int j;
                for (j = i; j < i + 2 * pairsNum; j += 2)
                {
                    strcat(str, argv[j]);
                    strcat(str, " ");
                    strcat(str, argv[j + 1]);
                    strcat(str, " ");
                }
                i = j;
                str[strlen(str) - 1] = 0;
                saveDiffToTmp(str);
                free(str);
                saveTmpToArr(mainarr);
            }
            else if (strcmp(argv[i], "remove_block") == 0)
            {
                if (argc <= i + 1)
                {
                    printf("You have to pass the block index!\n");
                    return 1;
                }
                int j = atoi(argv[i + 1]);
                i += 2;
                removeOpBlock(mainarr, j);
            }
            else if (strcmp(argv[i], "remove_operation") == 0)
            {
                if (argc <= i + 2)
                {
                    printf("You have to pass the block and the operation index!\n");
                    return 1;
                }
                int j = atoi(argv[i + 1]);
                int k = atoi(argv[i + 2]);
                i += 3;
                removeOp(mainarr, j, k);
            }
            else if (strcmp(argv[i], "free_array") == 0)
            {
                freeAll(mainarr);
                mainarr = NULL;
                i++;
            }
            else
            {
                printf("%s is a wrong argument!\n", argv[i]);
                return 1;
            }
        }
    }
    rtend = times(tend);
    print_time;
    free(tstart), free(tend);
    freeAll(mainarr);
    return 0;
}