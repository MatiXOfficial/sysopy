#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void generate(char *fileName, int recNum, int recLen)
{
    FILE *file = fopen(fileName, "w");
    FILE *randomFile = fopen("/dev/random", "r");
    for (int i = 0; i < recNum; i++)
    {
        for (int j = 0; j < recLen; j++)
        {
            char ch = getc(randomFile) % 93 + 33;
            putc(ch, file);
        }
        putc('\n', file);
    }
    fclose(randomFile);
    fclose(file);
}

////////////////////////////// sysSort ////////////////////////////////////
void sysSwap(int file, int recLen, int i, int j)
{
    if (i == j)
        return;
    char *buffer1 = calloc(recLen + 1, 1);
    char *buffer2 = calloc(recLen + 1, 1);
    lseek(file, i * recLen, 0);
    read(file, buffer1, recLen);
    lseek(file, j * recLen, 0);
    read(file, buffer2, recLen);
    lseek(file, j * recLen, 0);
    write(file, buffer1, recLen);
    lseek(file, i * recLen, 0);
    write(file, buffer2, recLen);
    free(buffer1);
    free(buffer2);
}

int sysPartition(int file, int recLen, int p, int r)
{
    char *pivot = calloc(recLen + 1, 1);
    char *buffer = calloc(recLen + 1, 1);
    lseek(file, p * recLen, 0);
    read(file, pivot, recLen);
    int i = p + 1, j = p + 1;
    while (i <= r)
    {
        read(file, buffer, recLen);
        if (strcmp(buffer, pivot) < 0)
        {
            sysSwap(file, recLen, i, j);
            j++;
        }
        i++;
    }
    j--;
    sysSwap(file, recLen, p, j);
    free(pivot);
    free(buffer);
    return j;
}

void sysQuickSort(int file, int recLen, int p, int r)
{
    if (p >= r)
        return;
    int q = sysPartition(file, recLen, p, r);
    sysQuickSort(file, recLen, p, q - 1);
    sysQuickSort(file, recLen, q + 1, r);
}

void sysSort(char *fileName, int recNum, int recLen)
{
    int file = open(fileName, O_RDWR);
    sysQuickSort(file, recLen + 1, 0, recNum - 1);
    close(file);
}

////////////////////////////// libSort ////////////////////////////////////
void libSwap(FILE *file, int recLen, int i, int j)
{
    if (i == j)
        return;
    char *buffer1 = calloc(recLen + 1, 1);
    char *buffer2 = calloc(recLen + 1, 1);
    fseek(file, i * recLen, 0);
    fread(buffer1, 1, recLen, file);
    fseek(file, j * recLen, 0);
    fread(buffer2, 1, recLen, file);
    fseek(file, j * recLen, 0);
    fwrite(buffer1, 1, recLen, file);
    fseek(file, i * recLen, 0);
    fwrite(buffer2, 1, recLen, file);
    free(buffer1);
    free(buffer2);
}

int libPartition(FILE *file, int recLen, int p, int r)
{
    char *pivot = calloc(recLen + 1, 1);
    char *buffer = calloc(recLen + 1, 1);
    fseek(file, p * recLen, 0);
    fread(pivot, 1, recLen, file);
    int i = p + 1, j = p + 1;
    while (i <= r)
    {
        fread(buffer, 1, recLen, file);
        if (strcmp(buffer, pivot) < 0)
        {
            libSwap(file, recLen, i, j);
            j++;
        }
        i++;
    }
    j--;
    libSwap(file, recLen, p, j);
    free(pivot);
    free(buffer);
    return j;
}

void libQuickSort(FILE *file, int recLen, int p, int r)
{
    if (p >= r)
        return;
    int q = libPartition(file, recLen, p, r);
    libQuickSort(file, recLen, p, q - 1);
    libQuickSort(file, recLen, q + 1, r);
}

void libSort(char *fileName, int recNum, int recLen)
{
    FILE *file = fopen(fileName, "r+");
    libQuickSort(file, recLen + 1, 0, recNum - 1);
    fclose(file);
}

///////////////////////////// copy ////////////////////////////////////////
void sysCopy(char *fileName1, char *fileName2, int recNum, int recLen)
{
    int file1 = open(fileName1, O_RDONLY);
    int file2 = open(fileName2, O_CREAT|O_WRONLY|O_TRUNC);
    char *buffer = calloc(recLen + 2, 1);
    while (read(file1, buffer, recLen + 1) > 0)
    {
        write(file2, buffer, recLen + 1);
    }
    free(buffer);
    close(file1);
    close(file2);
}

void libCopy(char *fileName1, char *fileName2, int recNum, int recLen)
{
    FILE *file1 = fopen(fileName1, "r");
    FILE *file2 = fopen(fileName2, "w");
    char *buffer = calloc(recLen + 2, 1);
    while(fread(buffer, 1, recLen + 1, file1))
    {
        fwrite(buffer, 1, recLen + 1, file2);
    }
    free(buffer);
    fclose(file1);
    fclose(file2);
}