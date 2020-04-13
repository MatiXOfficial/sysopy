#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define TIME_BREAK 2

void error(char *mes);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        error("wrong number of arguments, try ./consumer [pipe path] [text file] [number of chars]");
    }
    int pipe = open(argv[1], O_RDONLY);
    int textFile = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT);
    int N = atoi(argv[3]);

    printf("consumer starts working\n");
    char *extract = calloc(N + 1, sizeof(char));
    int idx = read(pipe, extract, N);
    while(idx > 0)
    {
        extract[idx] = '\0';
        write(textFile, extract, idx);
        printf("consumer: %s\n", extract);
        idx = read(pipe, extract, N);
    }

    free(extract);
    close(textFile);
    close(pipe);
}

//////////// Definitions /////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}