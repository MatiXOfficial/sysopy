#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void error(char *mes);

int main()
{
    system("./generator 5");
    for (int i = 0; i < 5; i++)
    {
        if (fork() == 0)
        {
            char textFileName[20];
            char num[3];
            sprintf(textFileName, "prod_text%d.txt", i + 1);
            sprintf(num, "%d", (13 * i) % 7 + 3);
            execlp("./producer", "producer", "/tmp/pipe", textFileName, num, NULL);
            error("producer execution error");
        }
    }
    if (fork() == 0)
    {
        execlp("./consumer", "consumer", "/tmp/pipe", "results.txt", "3", NULL);
        error("consumer execution error");
    }
    for (int i = 0; i < 6; i++)
    {
        wait(NULL);
    }
}

//////////// Definitions /////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}