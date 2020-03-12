#include "difflib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct MainArray *createMainArray(int n)
{
    struct MainArray *mainarr = calloc(1, sizeof(struct MainArray));
    mainarr->oparr = calloc(n, sizeof(struct OperationsArray));
    mainarr->length = 0;
    mainarr->n = n;
    return mainarr;
}

void saveDiffToTmp(char *filesString)
{
    // Counting files
    int filesCount = 1;
    for (int i = 0; filesString[i] != EOF; i++)
        if (filesString[i] == ' ')
            filesCount++;

    if (strlen(filesString) == 0 || filesCount == 0 || filesCount % 2 == 1)
    {
        printf("Wrong files number!\n");
    }

    // Reading file names
    char *newFilesString = calloc(strlen(filesString), sizeof(char));
    strcpy(newFilesString, filesString);
    char **filesSequence = calloc(filesCount, sizeof(char *));
    char *file = strtok(newFilesString, " ");
    for (int i = 0; i < filesCount; i++)
    {
        filesSequence[i] = calloc(strlen(file), sizeof(char));
        strcpy(filesSequence[i], file);
        file = strtok(NULL, " ");
    }

    // Executing proper diff-s and saving results to tmp file
    FILE *tmpFile = fopen("diffTmpFile.txt", "w+");
    char pairsCount[3];
    sprintf(pairsCount, "%d\n", filesCount / 2);
    fputs(pairsCount, tmpFile);
    char command[50], ch;
    for (int i = 0; i < filesCount; i += 2)
    {
        sprintf(command, "diff %s %s", filesSequence[i], filesSequence[i + 1]);
        FILE *diffout = popen(command, "r");
        ch = getc(diffout);
        while(ch != EOF)
        {
            fputc(ch, tmpFile);
            ch = getc(diffout);
        }
        putc('\n', tmpFile);
        pclose(diffout);
    }
    pclose(tmpFile);

    for (int i = 0; i < filesCount; i++)
        free(filesSequence[i]);
    free(filesSequence);

    printf("Saved results to the temporary file.\n");
}

int saveTmpToArr(struct MainArray *mainarr)
{
    FILE *tmpFile = fopen("diffTmpFile.txt", "r+");
    if (tmpFile == NULL)
    {
        printf("No tmp file.\n");
        return -1;
    }

    // Counting operations numbers
    char str[100];
    fgets(str, 100, tmpFile);
    int pairsNum;
    sscanf(str, "%d", &pairsNum);
    if (mainarr->length + pairsNum > mainarr->n)
    {
        printf("Not enoguh space in the main array!");
        return -1;
    }
    mainarr->length += pairsNum;

    for (int i = mainarr->length - pairsNum; i < mainarr->length; i++)
    {
        mainarr->oparr[i] = calloc(1, sizeof(struct OperationsArray));
        mainarr->oparr[i]->length = 0;
    }
    int i = mainarr->length - pairsNum;
    while (fgets(str, 100, tmpFile) != NULL)
    {
        if (isdigit(str[0]))
            mainarr->oparr[i]->length++;
        else if (str[0] == '\n')
            i++;
    }

    // Filling operations array
    rewind(tmpFile);
    char tmpOpStr[500] = "";
    fgets(str, 100, tmpFile);
    i = mainarr->length - pairsNum; int j = 0;
    while (fgets(str, 100, tmpFile) != NULL)
    {
        if (isdigit(str[0]))
        {
            if (strlen(tmpOpStr) == 0)
            {
                mainarr->oparr[i]->op = calloc(mainarr->oparr[i]->length, sizeof(char *));
            }
            else
            {
                mainarr->oparr[i]->op[j] = calloc(strlen(tmpOpStr), sizeof(char));
                strcpy(mainarr->oparr[i]->op[j], tmpOpStr);
                strcpy(tmpOpStr, "");
                j++;
            }
            strcat(tmpOpStr, str);
        }
        else if (str[0] == '\n')
        {
            mainarr->oparr[i]->op[j] = calloc(strlen(tmpOpStr), sizeof(char));
            strcpy(mainarr->oparr[i]->op[j], tmpOpStr);
            strcpy(tmpOpStr, "");
            i++;
            j = 0;
        }
        else
        {
            strcat(tmpOpStr, str);
        }
    }
    fclose(tmpFile);
    return mainarr->length - 1;
}

int opNum(struct MainArray *mainarr, int i)
{
    if (i >= mainarr->length)
    {
        printf("Wrong index!\n");
        return -1;
    }
    return mainarr->oparr[i]->length;
}

void removeOpBlock(struct MainArray *mainarr, int i)
{
    if (i >= mainarr->length)
    {
        printf("Wrong index!\n");
        return;
    }
    for (int j = 0; j < mainarr->oparr[i]->length; j++)
    {
        free(mainarr->oparr[i]->op[j]);
    }
    free(mainarr->oparr[i]->op);
    free(mainarr->oparr[i]);
    mainarr->length--;
    for (int j = i; j < mainarr->length; j++)
    {
        mainarr->oparr[j] = mainarr->oparr[j + 1];
    }
    mainarr->oparr = realloc(mainarr->oparr, mainarr->length * sizeof(struct OperationsArray *));
}

void removeOp(struct MainArray *mainarr, int i, int j)
{
    if (i >= mainarr->length || j >= mainarr->oparr[i]->length)
    {
        printf("Wrong index!\n");
        return;
    }
    free(mainarr->oparr[i]->op[j]);
    mainarr->oparr[i]->length--;
    for (int k = j; k < mainarr->oparr[i]->length; k++)
    {
        mainarr->oparr[i]->op[k] = mainarr->oparr[i]->op[k + 1];
    }
    mainarr->oparr[i]->op = realloc(mainarr->oparr[i]->op, mainarr->oparr[i]->length * sizeof(char *));
}

void freeAll(struct MainArray *mainarr)
{
    if (mainarr == NULL)
    {
        return;
    }
    for (int i = 0; i < mainarr->length; i++)
    {
        for (int j = 0; j < mainarr->oparr[i]->length; j++)
        {
            free(mainarr->oparr[i]->op[j]);
        }
        free(mainarr->oparr[i]->op);
        free(mainarr->oparr[i]);
    }
    free(mainarr->oparr);
    free(mainarr);
}