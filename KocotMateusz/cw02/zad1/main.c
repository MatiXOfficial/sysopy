#include "iooperations.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

double calTime(clock_t start, clock_t end)
{
    return ((double) (end - start)) / sysconf(_SC_CLK_TCK);
}

void printTime(double stime, double utime)
{
    printf("s: %f, u: %f\n", stime, utime);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("You have to choose either generate, sort or copy!\n");
        return 1;
    }
    struct tms *tstart = calloc(1, sizeof(struct tms)), *tend = calloc(1, sizeof(struct tms));
    times(tstart);
    if (strcmp(argv[1], "generate") == 0)
    {
        if (argc != 5)
        {
            printf("Wrong! Use ./main generate [fileName] [recNum] [recLen] instead!\n");
        }
        else
        {
            generate(argv[2], atoi(argv[3]), atoi(argv[4]));
        }
    }
    else if (strcmp(argv[1], "sort") == 0)
    {
        if (argc != 6)
        {
            printf("Wrong! Use ./main sort [fileName] [recNum] [recLen] [sys/lib] instead!\n");
        }
        else if (strcmp(argv[5], "sys") == 0)
        {
            sysSort(argv[2], atoi(argv[3]), atoi(argv[4]));
        }
        else if (strcmp(argv[5], "lib") == 0)
        {
            libSort(argv[2], atoi(argv[3]), atoi(argv[4]));
        }
        else
        {
            printf("Fifth argument should be either sys or lib!\n");
        }
    }
    else if (strcmp(argv[1], "copy") == 0)
    {
        if (argc != 7)
        {
            printf("Wrong! Use ./main copy [fileFrom] [fileTo] [recNum] [recLen] [sys/lib] instead!\n");
        }
        else if (strcmp(argv[6], "sys") == 0)
        {
            sysCopy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
        }
        else if (strcmp(argv[6], "lib") == 0)
        {
            libCopy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
        }
        else
        {
            printf("Sixth argument should be either sys or lib!\n");
        }
    }
    else
    {
        printf("Wrong argument!\n");
    }
    times(tend);
    printTime(calTime(tstart->tms_stime, tend->tms_stime), calTime(tstart->tms_utime, tend->tms_utime));
    free(tstart), free(tend);
}