#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>

void error(char *mes);

int checkNotDelim(char sign, char *delims, int n);
void fileTok(char *str, int fd, char *delims);
int fileTokNumber(int fd, char *delims);
double getTimeDiff(struct timeval start, struct timeval end);

struct SignThreadArgs
{
    int min;
    int max;
    int idx;
};

void *histPartSign(void *ptr);
void *histPartBlock(void *ptr);
void *histPartInterleaved(void *ptr);

int **matrix;
int rows, cols;
int **hist;
pthread_t *threadIds;

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        error("Wrong number of arguments. Try ./main threadNum [sign/block/interleaved] inputFileName outputFileName");
    }
    int threadNum = atoi(argv[1]);

    int inputFile = open(argv[3], O_RDONLY);
    fileTok(NULL, inputFile, "\n");

    cols = fileTokNumber(inputFile, " ");
    rows = fileTokNumber(inputFile, "\n");
    int M = fileTokNumber(inputFile, "\n") + 1;

    matrix = calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = calloc(cols, sizeof(int));
        for (int j = 0; j < cols; j++)
        {
            matrix[i][j] = fileTokNumber(inputFile, " \n");
        }
    }
    close(inputFile);

    hist = calloc(threadNum, sizeof(int *));
    for (int i = 0; i < threadNum; i++)
    {
        hist[i] = calloc(M, sizeof(int));
    }
    threadIds = calloc(threadNum, sizeof(pthread_t));
    double *times = calloc(threadNum, sizeof(double));
    struct timeval start, end;

    // Histogram finding
    gettimeofday(&start, NULL);
    if (strcmp(argv[2], "sign") == 0)
    {
        int divSize = (M + threadNum - 1) / threadNum;
        int min = 0, max = 0;
        for (int i = 0; i < threadNum; i++)
        {
            min = max;
            max = min + divSize;
            if (max >= M)
            {
                max = M;
            }
            struct SignThreadArgs *threadArgs = calloc(1, sizeof(struct SignThreadArgs));
            threadArgs->min = min;
            threadArgs->max = max;
            pthread_create(&threadIds[i], NULL, histPartSign, (void *) threadArgs);
        }
    }
    else if (strcmp(argv[2], "block") == 0)
    {
        int divSize = (cols + threadNum - 1) / threadNum;
        int min = 0, max = 0;   
        for (int i = 0; i < threadNum; i++)
        {
            min = max;
            max = min + divSize;
            if (max >= cols)
            {
                max = cols;
            }
            struct SignThreadArgs *threadArgs = calloc(1, sizeof(struct SignThreadArgs));
            threadArgs->min = min;
            threadArgs->max = max;
            threadArgs->idx = i;
            pthread_create(&threadIds[i], NULL, histPartBlock, (void *) threadArgs);
        }
    }
    else if (strcmp(argv[2], "interleaved") == 0)
    {
        int max = threadNum;    // min - starting column, max - step
        for (int min = 0; min < threadNum; min++)
        {
            struct SignThreadArgs *threadArgs = calloc(1, sizeof(struct SignThreadArgs));
            threadArgs->min = min;
            threadArgs->max = max;
            pthread_create(&threadIds[min], NULL, histPartInterleaved, (void *) threadArgs);
        }
    }
    else
    {
        error("Wrong second argument. Use [sign/block/interleaved]");
    }

    // Receiving times
    for (int i = 0; i < threadNum; i++)
    {
        void *tmpTime;
        pthread_join(threadIds[i], &tmpTime);
        times[i] = *((double *) tmpTime);
        free(tmpTime);
    }
    if (strcmp(argv[2], "sign") != 0)
    {
        for (int i = threadNum - 1; i > 0; i--)
        {
            for (int j = 0; j < M; j++)
            {
                hist[0][j] += hist[i][j];
            }
        }
    }
    gettimeofday(&end, NULL);
    free(threadIds);

    // Printing times
    for (int i = 0; i < threadNum; i++)
    {
        printf("Thread no: %2d: %.0f µs\n", i + 1, times[i]);
    }
    printf("Total time: %.0f µs\n", getTimeDiff(start, end));
    free(times);

    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);

    // Printing histogram to the file
    int outputFile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    char buffer[64];
    for (int i = 0; i < M; i++)
    {
        sprintf(buffer, "%d : %d\n", i, hist[0][i]);
        write(outputFile, buffer, strlen(buffer));
    }
    close(outputFile);
    for (int i = 0; i < threadNum; i++)
    {
        free(hist[i]);
    }
    free(hist);
}


////////////// Definitions ////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}

int checkNotDelim(char sign, char *delims, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (sign == delims[i])
        {
            return 0;
        }
    }
    if (sign == 13)
    {
        return 0;
    }
    return 1;
}

void fileTok(char *str, int fd, char *delims)
{
    int n = strlen(delims);
    char sign;

    read(fd, &sign, 1);
    while (sign != EOF && checkNotDelim(sign, delims, n) == 0)
    {
        read(fd, &sign, 1);
    }

    int i = 0;
    while (sign != EOF && checkNotDelim(sign, delims, n) == 1)
    {
        if (str != NULL)
        {
            str[i] = sign;
        }
        i++;
        read(fd, &sign, 1);
    }
    if (str != NULL)
    {
        str[i] = '\0';
    }
}

int fileTokNumber(int fd, char *delims)
{
    char buffer[8];
    fileTok(buffer, fd, delims);
    return atoi(buffer);
}

double getTimeDiff(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
}

void *histPartSign(void *ptr)
{
    struct timeval start, end;

    struct SignThreadArgs *threadArgs = (struct SignThreadArgs *)ptr;
    int min = threadArgs->min;
    int max = threadArgs->max;

    gettimeofday(&start, NULL);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (matrix[i][j] >= min && matrix[i][j] < max)
            {
                hist[0][matrix[i][j]]++;
            }
        }
    }
    gettimeofday(&end, NULL);
    free(ptr);

    double *time = calloc(1, sizeof(double));
    *time = getTimeDiff(start, end);
    pthread_exit((void *)time);
}

void *histPartBlock(void *ptr)
{
    struct timeval start, end;

    struct SignThreadArgs *threadArgs = (struct SignThreadArgs *)ptr;
    int min = threadArgs->min;
    int max = threadArgs->max;
    int threadIdx = threadArgs->idx;

    gettimeofday(&start, NULL);
    for (int i = 0; i < rows; i++)
    {
        for (int j = min; j < max; j++)
        {
            hist[threadIdx][matrix[i][j]]++;
        }
    }
    gettimeofday(&end, NULL);
    free(ptr);

    double *time = calloc(1, sizeof(double));
    *time = getTimeDiff(start, end);
    pthread_exit((void *)time);
}

void *histPartInterleaved(void *ptr)
{
    struct timeval start, end;

    struct SignThreadArgs *threadArgs = (struct SignThreadArgs *)ptr;
    int min = threadArgs->min;
    int step = threadArgs->max;

    gettimeofday(&start, NULL);
    for (int i = 0; i < rows; i++)
    {
        for (int j = min; j < cols; j += step)
        {
            hist[min][matrix[i][j]]++;
        }
    }
    gettimeofday(&end, NULL);
    free(ptr);

    double *time = calloc(1, sizeof(double));
    *time = getTimeDiff(start, end);
    pthread_exit((void *)time);
}