#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(char *mes);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("wrong arguments number");
    }
    char dir[100];
    strcpy(dir, argv[1]);

    char command[200];
    sprintf(command, "%s %s", "sort", dir);

    FILE *file = popen(command, "r");

    char buffer[1000];
    fread(buffer, sizeof(char), 1000, file);
    printf("%s\n", buffer);

    pclose(file);
}

//////////// Definitions //////////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}