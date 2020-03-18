#include "iolib.h"
#include "iolib.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("You have to choose either generate, sort or copy!\n");
        return 1;
    }
    if (strcmp(argv[1], "generate") == 0)
    {
        if (argc != 5)
        {
            printf("Wrong arguments number!\n");
            return 1;
        }
        generate(argv[2], atoi(argv[3]), atoi(argv[4]));
    }
    else if (strcmp(argv[1], "sort") == 0)
    {
        if (argc != 6)
        {
            printf("Wrong arguments number!\n");
            return 1;
        }
        if (strcmp(argv[5], "sys") == 0)
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
            return 1;
        }
    }
    else if (strcmp(argv[1], "copy") == 0)
    {
        if (argc != 7)
        {
            printf("Wrong arguments number!\n");
            return 1;
        }
        if (strcmp(argv[6], "sys") == 0)
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
            return 1;
        }
    }
    else
    {
        printf("Wrong argument!\n");
        return 1;
    }
    return 0;
}