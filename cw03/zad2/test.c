#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int NUMLEN = 16;

struct Matrix
{
    int nrow;
    int ncol;
    int **val;
};

void error(char *mes);
void fileTok(FILE *file, char *buffer, char *delim);
struct Matrix readMatrix(FILE *file);
int multiplyRowCol(struct Matrix matrixIn1, struct Matrix matrixIn2, int row, int col);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("Wrong arguments! Try: ./multi [list file]");
    }
    FILE *listFile = fopen(argv[1], "r");
    if (listFile == NULL)
    {
        error("Wrong list file!");
    }
    char buffer[512];
    fileTok(listFile, buffer, "\n");
    int multiCount = atoi(buffer);
    printf("%d\n", multiCount);
    int succFlag = 1;

    for (int i = 0; i < multiCount; i++)
    {
        fileTok(listFile, buffer, " ");
        FILE *fileIn1 = fopen(buffer, "r");
        fileTok(listFile, buffer, " ");
        FILE *fileIn2 = fopen(buffer, "r");
        fileTok(listFile, buffer, "\n");
        FILE *fileOut = fopen(buffer, "r");
        if (fileIn1 == NULL || fileIn2 == NULL || fileOut == NULL)
        {
            error("Wrong file name!");
        }

        struct Matrix matrixIn1 = readMatrix(fileIn1);
        struct Matrix matrixIn2 = readMatrix(fileIn2);

        fclose(fileIn1);
        fclose(fileIn2);

        char buffer[16];
        int contFlag = 1;
        for (int j = 0; j < matrixIn1.nrow; j++)
        {
            for (int k = 0; k < matrixIn2.ncol; k++)
            {
                fileTok(fileOut, buffer, " \n");
                if (multiplyRowCol(matrixIn1, matrixIn2, j, k) != atoi(buffer))
                {
                    printf("%d: ERROR\n", i);
                    contFlag = 0;
                    succFlag = 0;
                    break;
                }
            }
            if (contFlag == 0)
            {
                break;
            }
        }
    }
    if (succFlag == 1)
    {
        printf("SUCCESS!\n");
    }
}

///////////////////// Defitintions ////////////////////////////////////////
void error(char *mes)
{
    perror(mes);
    _exit(1);
}

void fileTok(FILE *file, char* buffer, char *delim)
{
    char ch = getc(file);
    int i = 0;
    int contFlag = 1;
    while (ch != EOF)
    {
        for (int j = 0; delim[j] != '\0'; j++)
        {
            if (ch == delim[j])
            {
                contFlag = 0;
                break;
            }
        }
        if (contFlag == 0)
        {
            break;
        }
        buffer[i] = ch;
        ch = getc(file);
        i++;
    }
    buffer[i] = '\0';
    while(ch != EOF)
    {
        ch = getc(file);
        contFlag = 0;
        for (int j = 0; delim[j] != '\0'; j++)
        {
            if (ch == delim[j])
            {
                contFlag = 1;
                break;
            }
        }
        if (contFlag == 0)
        {
            fseek(file, -1, SEEK_CUR);
            break;
        }
    }
}

struct Matrix readMatrix(FILE *file)
{
    fseek(file, 0, 0);
    char buffer[16];
    fileTok(file, buffer, "x");
    int nrow = atoi(buffer);
    fileTok(file, buffer, "\n");
    int ncol = atoi(buffer);

    struct Matrix matrix;
    matrix.nrow = nrow;
    matrix.ncol = ncol;
    matrix.val = calloc(nrow, sizeof(int *));
    for (int i = 0; i < nrow; i++)
    {
        matrix.val[i] = calloc(ncol, sizeof(int));
        for (int j = 0; j < ncol; j++)
        {
            fileTok(file, buffer, " \n");
            matrix.val[i][j] = atoi(buffer);
        }
    }
    return matrix;
}

int multiplyRowCol(struct Matrix matrixIn1, struct Matrix matrixIn2, int row, int col)
{
    int limit = matrixIn1.ncol;
    int result = 0;
    for (int i = 0; i < limit; i++)
    {
        result += matrixIn1.val[row][i] * matrixIn2.val[i][col];
    }
    return result;
}