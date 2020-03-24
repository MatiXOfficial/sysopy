#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <time.h>
#include <sys/times.h>
#include <sys/stat.h>

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
void generateProcesses(int *pidArr, int procNum, int *exitArr);
int getpidx(int *pidArr, int pid, int procNum);
int multiplyRowCol(struct Matrix matrixIn1, struct Matrix matrixIn2, int row, int col);
void multiplyShared(struct Matrix matrixIn1, struct Matrix matrixIn2, FILE *fileOut, int procNum, int *pidArr, int step, int *multFrag);
void multiplySeparate(struct Matrix matrixIn1, struct Matrix matrixIn2, FILE *fileOut, int procNum, int *pidArr, int step, int *multFrag);

int main(int argc, char *argv[])
{
    //////////////// Arguments /////////////////////////////////////
    if (argc != 5)
    {
        error("Wrong arguments! Try: ./multi [list file] [processes count] [time limit] [shared/separate (result file)]");
    }

    FILE *listFile = fopen(argv[1], "r");
    if (listFile == NULL)
    {
        error("Wrong list file!");
    }
    char buffer[512];
    fileTok(listFile, buffer, "\n");
    int multiCount = atoi(buffer);
    int procNum = atoi(argv[2]);
    if (procNum <= 0)
    {
        error("Wrong processes count!");
    }
    float timeLimit = atof(argv[3]);
    int useShared = 0;
    if (strcmp(argv[4], "shared") == 0)
    {
        useShared = 1;
    }
    else if (strcmp(argv[4], "separate") != 0)
    {
        error("4th argument should be either shared or separate (result file)");
    }
    

    ///////////////// Program ////////////////////////////////////////
    int mainPid = getpid();
    int *pidArr = calloc(procNum, sizeof(int));
    int *exitArr = calloc(procNum, sizeof(int));
    generateProcesses(pidArr, procNum, exitArr);
    int multFragm = 0;
    clock_t tstart = times(NULL);

    if (getpid() != mainPid)
    {
        for (int i = 0; i < multiCount; i++)
        {
            clock_t tend = times(NULL);
            if ((double) (tend - tstart) / sysconf(_SC_CLK_TCK) > timeLimit)
            {
                exit(multFragm);
            }
            fileTok(listFile, buffer, " ");
            FILE *fileIn1 = fopen(buffer, "r");
            fileTok(listFile, buffer, " ");
            FILE *fileIn2 = fopen(buffer, "r");
            if (fileIn1 == NULL || fileIn2 == NULL)
            {
                error("Wrong file name!");
            }
            fileTok(listFile, buffer, "\n");

            struct Matrix matrixIn1 = readMatrix(fileIn1);
            struct Matrix matrixIn2 = readMatrix(fileIn2);

            int step = (matrixIn2.ncol + procNum - 1) / procNum;

            if (useShared == 1)
            {
                FILE *fileOut = fopen(buffer, "r+");
                if (fileOut == NULL)
                {
                    fileOut = fopen(buffer, "w");
                }

                multiplyShared(matrixIn1, matrixIn2, fileOut, procNum, pidArr, step, &multFragm);

                fclose(fileOut);
            }
            else
            {
                FILE *fileOut = fopen(buffer, "w");
                fclose(fileOut);
                int pidx = getpidx(pidArr, getpid(), procNum);
                sprintf(buffer, "%s%d", buffer, pidx);

                fileOut = fopen(buffer, "r+");
                if (fileOut == NULL)
                {
                    fileOut = fopen(buffer, "w");
                }

                multiplySeparate(matrixIn1, matrixIn2, fileOut, procNum, pidArr, step, &multFragm);

                fclose(fileOut);
            }

            free(matrixIn1.val);
            free(matrixIn2.val);
            fclose(fileIn1);
            fclose(fileIn2);
        }
        exit(multFragm);
    }
    else if (useShared == 0)
    {   
        for (int i = 0; i < multiCount; i++)
        {
            fileTok(listFile, buffer, " ");
            fileTok(listFile, buffer, " ");
            fileTok(listFile, buffer, "\n");

            char **command = calloc(procNum + 4, sizeof(char *));
            if (fork() == 0)
            {
                command[0] = calloc(strlen("paste") + 1, 1);    strcpy(command[0], "paste");
                command[1] = calloc(strlen("-d") + 1, 1);       strcpy(command[1], "-d");
                command[2] = calloc(strlen("") + 1, 1);                      strcpy(command[2], "");
                for (int j = 3; j < procNum + 3; j++)
                {
                    char newbuffer[512];
                    sprintf(newbuffer, "%s%d", buffer, j - 3);
                    command[j] = calloc(strlen(newbuffer) + 1, 1);      strcpy(command[j], newbuffer);
                }
                command[procNum + 3] = NULL;

                int newFileOut = open(buffer, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(newFileOut, 1);
                close(newFileOut);

                execvp("paste", command);
                printf("paste ");
                for (int i = 0; i < procNum + 4; i++)
                    printf("%s ", command[i]);
                printf("\n");
                exit(0);
            }
            else
            {
                wait(NULL);
                for (int j = 0; j < procNum + 4; j++)
                {
                    free(command[j]);
                }
                free(command);
            }
            
            for (int j = 0; j < procNum; j++)
            {
                char newbuffer[512];
                sprintf(newbuffer, "%s%d", buffer, j);
                if (fork() == 0)
                {
                    execlp("rm", "rm", "-f", newbuffer, NULL);
                }
                else
                {
                    wait(NULL);
                }
            }
        }
    }

    for (int i = 0; i < procNum; i++)
    {
        printf("Process nr: %d, pid: %d multiplied the matrices %d time(s)\n", i, pidArr[i], exitArr[i]);
    }

    free(pidArr);
    free(exitArr);
    fclose(listFile);
}

///////////////// Definitions ///////////////////////////////////////////////////
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

int getpidx(int *pidArr, int pid, int procNum)
{
    int pidx = -1;
    for (int i = 0; i < procNum; i++)
    {
        if (getpid() == pidArr[i])
        {
            pidx = i;
        }
    }
    return pidx;
}

void generateProcesses(int *pidArr, int procNum, int *exitArr)
{
    int childPid = 1;
    for (int i = 0; i < procNum; i++)
    {
        if (childPid != 0)
        {
            childPid = fork();
        }
        if (childPid != 0)
        {
            pidArr[i] = childPid;
        }
        else
        {
            pidArr[i] = getpid();
            break;
        }
    }
    if (childPid != 0)
    {
        for (int i = 0; i < procNum; i++)
        {
            int status;
            waitpid(pidArr[i], &status, 0);
            exitArr[i] = WEXITSTATUS(status);
        }
    }
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

void multiplyShared(struct Matrix matrixIn1, struct Matrix matrixIn2, FILE *fileOut, int procNum, int *pidArr, int step, int *multFrag)
{
    int pidx = getpidx(pidArr, getpid(), procNum);
    if (pidx * step >= matrixIn2.ncol)
    {
        return;
    }
    char buffer[16];
    flock(fileno(fileOut), LOCK_EX);
    for (int i = 0; i < matrixIn1.nrow; i++)
    {
        for (int j = pidx * step; j < matrixIn2.ncol && j < (pidx + 1) * step; j++)
        {
            int result = multiplyRowCol(matrixIn1, matrixIn2, i, j);
            sprintf(buffer, "%d", result);
            int len = strlen(buffer);
            fseek(fileOut, i * (matrixIn2.ncol * NUMLEN + 1) + j * NUMLEN, 0);
            fwrite(buffer, 1, len, fileOut);
            for (int i = len; i < NUMLEN; i++)
            {
                putc(' ', fileOut);
            }
            if (j == matrixIn2.ncol - 1)
            {
                putc('\n', fileOut);
            }
        }
    }
    flock(fileno(fileOut), LOCK_UN);
    (*multFrag)++;
}

void multiplySeparate(struct Matrix matrixIn1, struct Matrix matrixIn2, FILE *fileOut, int procNum, int *pidArr, int step, int *multFrag)
{
    int pidx = getpidx(pidArr, getpid(), procNum);
    if (pidx * step >= matrixIn2.ncol)
    {
        return;
    }
    char buffer[16];
    for (int i = 0; i < matrixIn1.nrow; i++)
    {
        for (int j = pidx * step; j < matrixIn2.ncol && j < (pidx + 1) * step; j++)
        {
            int result = multiplyRowCol(matrixIn1, matrixIn2, i, j);
            sprintf(buffer, "%d", result);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, fileOut);
            for (int i = len; i < NUMLEN; i++)
            {
                putc(' ', fileOut);
            }
        }
        putc('\n', fileOut);
    }
    (*multFrag)++;
}