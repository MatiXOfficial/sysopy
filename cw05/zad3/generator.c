/*
Generates a named pipe in /tmp/pipe.
Generates a consumer's result text file in ./results.txt.
Generates PN producers in ./prod_text1.txt, ./prod_text2.txt, ... , ./prod_text[PN].txt.
Every producer receives a range of possible characters between 33 and 126 in ASCII.
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define RANGE_START 1
#define RANGE_END 20

void error(char *mes);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("wrong number of arguments. try ./generator [producers count]");
    }
    int PN = atoi(argv[1]);
    system("rm -f /tmp/pipe");
    if (mkfifo("/tmp/pipe", 0777) == -1)
    {
        error("mkfifo error");
    }

    srand(time(NULL));
    int delta = (126 - 33 + 1) / PN;
    for (int i = 0; i < PN; i++)
    {
        char name[20];
        sprintf(name, "prod_text%d.txt", i + 1);
        int file = open(name, O_WRONLY | O_CREAT | O_TRUNC);
        int length = rand() % (RANGE_START - RANGE_END) + RANGE_START;
        for (int j = 0; j < length; j++)
        {
            int start = 33 + delta * i;
            char ch[] = {rand() % delta + start, '\0'};
            write(file, ch, 1);
        }
        close(file);
    }
}

//////////// Definitions /////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}