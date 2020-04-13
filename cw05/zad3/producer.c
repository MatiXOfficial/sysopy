#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TIME_BREAK 2

void error(char *mes);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        error("wrong number of arguments, try ./producer [pipe path] [text file] [number of chars]");
    }
    int pipe = open(argv[1], O_WRONLY);
    int textFile = open(argv[2], O_RDONLY);
    int N = atoi(argv[3]);
    pid_t pid = getpid();

    char *extract = calloc(N + 1, sizeof(char));
    char *message = calloc(N + 8, sizeof(char));
    int idx = read(textFile, extract, N);
    while (idx > 0)
    {
        extract[idx] = '\0';
        sleep(TIME_BREAK);
        sprintf(message, "#%d#%s", pid, extract);
        write(pipe, message, strlen(message));      // Writing to pipe
        printf("producer %d: %s\n", pid, message);
        idx = read(textFile, extract, N);
    }

    free(extract);
    free(message);
    close(textFile);
    close(pipe);
}

//////////// Definitions /////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}