#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

const int NUMLEN = 16;
const int MAXNUM = 100;

void error(char *mes);

int main(int argc, char *argv[])
{
    if (argc != 4 && argc != 5)
    {
        error("Wrong arguments! Try ./generator [n] [min size] [max size]");
    }
    int n = atoi(argv[1]);
    int minSize = atoi(argv[2]);
    int maxSize = atoi(argv[3]);
    int seed = 0;
    if (argc == 5)
        seed = atoi(argv[4]);
    else
        seed = time(0);
    mkdir("matrices", S_IRWXU);
    FILE *listFile = fopen("list", "w");
    char buffer[512];
    sprintf(buffer, "%d\n", n);
    fwrite(buffer, strlen(buffer), 1, listFile);

    srand(seed);
    for (int i = 0; i < n; i++)
    {
        sprintf(buffer, "matrices/m%d", i);
        mkdir(buffer, S_IRWXU);
        
        sprintf(buffer, "matrices/m%d/in1", i);
        FILE *file1 = fopen(buffer, "w");
        fwrite(buffer, strlen(buffer), 1, listFile);
        fwrite(" ", 1, 1, listFile);

        sprintf(buffer, "matrices/m%d/in2", i);
        FILE *file2 = fopen(buffer, "w");
        fwrite(buffer, strlen(buffer), 1, listFile);
        fwrite(" ", 1, 1, listFile);

        sprintf(buffer, "matrices/m%d/out", i);
        fwrite(buffer, strlen(buffer), 1, listFile);
        fwrite("\n", 1, 1, listFile);

        int dim1 = rand() % (maxSize - minSize) + minSize;
        int dim2 = rand() % (maxSize - minSize) + minSize;
        int dim3 = rand() % (maxSize - minSize) + minSize;

        sprintf(buffer, "%dx%d\n", dim1, dim2);
        fwrite(buffer, strlen(buffer), 1, file1);
        for (int j = 0; j < dim1; j++)
        {
            for (int k = 0; k < dim2; k++)
            {
                int num = rand() % MAXNUM;
                sprintf(buffer, "%d", num);
                fwrite(buffer, strlen(buffer), 1, file1);
                if (k < dim2 - 1)
                {
                    fwrite(" ", 1, 1, file1);
                }
            }
            fwrite("\n", 1, 1, file1);
        }

        sprintf(buffer, "%dx%d\n", dim2, dim3);
        fwrite(buffer, strlen(buffer), 1, file2);
        for (int j = 0; j < dim2; j++)
        {
            for (int k = 0; k < dim3; k++)
            {
                int num = rand() % rand() % MAXNUM;
                sprintf(buffer, "%d", num);
                fwrite(buffer, strlen(buffer), 1, file2);
                if (k < dim3 - 1)
                {
                    fwrite(" ", 1, 1, file2);
                }
            }
            fwrite("\n", 1, 1, file2);
        }
        
        fclose(file1);
        fclose(file2);
    }
    fclose(listFile);
}

void error(char *mes)
{
    perror(mes);
    _exit(1);
}