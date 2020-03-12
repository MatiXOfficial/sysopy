#include "difflib.h"
#include "difflib.c"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("You passed no arguments!\n");
        return 1;
    }
    struct MainArray *mainarr = NULL;
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
                char str[100] = "";
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
    if (mainarr != NULL)
    {
        for (int i = 0; i < mainarr->length; i++)
        {
            for (int j = 0; j < mainarr->oparr[i]->length; j++)
            {
                printf("%s\n", mainarr->oparr[i]->op[j]);
            }
        }
    }
    freeAll(mainarr);
    return 0;
}