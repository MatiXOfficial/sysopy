#include "difflib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct MainArray *createMainArray()
{
    struct MainArray *mainArray = calloc(1, sizeof(struct MainArray));
    mainArray->length = 0;
}

void diffTmp(char *filesString)
{
    int filesCount = 1;
    for (int i = 0; filesString[i] != EOF; i++)
        if (filesString[i] == ' ')
            filesCount++;

    if (strlen(filesString) == 0 || filesCount == 0 || filesCount % 2 == 1)
    {
        printf("Wrong files number!\n");
        return;
    }

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

    FILE *tmpFile = fopen("diffTmpFile.txt", "w+");
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

int main()
{
    diffTmp("a.txt b.txt c.txt d.txt");
}