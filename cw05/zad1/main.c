#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARGS_LIMIT 10
#define COMM_LIMIT 10

void error(char *mes);
void filetok(FILE *file, char *dest, char *delims);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("wrong arguments number");
    }
    char fileDir[100];
    strcpy(fileDir, argv[1]);

    FILE *file = fopen(fileDir, "r");

    char line[(ARGS_LIMIT + 1) * 200];
    char *command[ARGS_LIMIT + 1];
    for (int i = 0; i < ARGS_LIMIT + 1; i++)
    {
        command[i] = calloc(100, sizeof(char));
    }
    char *buffer;

    filetok(file, line, "\n");
    while(line[0] != EOF)
    {
        printf("\n---->current line: %s\n", line);
        int fd_prev[2]; 
        pipe(fd_prev);
        int fd_curr[2]; 
        pipe(fd_curr);
        bool isFirst = true;
        int procCount = 0;

        buffer = strtok(line, " ");
        int argidx = 0;
        while(buffer != NULL)
        {
            if (buffer[0] == '|')
            {
                free(command[argidx]);
                command[argidx] = NULL;
            }
            else
            {
                strcpy(command[argidx], buffer);
                argidx++;
            }
            buffer = strtok(NULL, " ");
            if (buffer == NULL)
            {
                free(command[argidx]);
                command[argidx] = NULL;
            }

            //////////// Main functionality //////////////
            if (command[argidx] == NULL)
            {
                if (fork() == 0)
                {
                    if (buffer != NULL)
                    {
                        dup2(fd_curr[1], STDOUT_FILENO);
                        close(fd_curr[0]);
                        close(fd_curr[1]);
                    }
                    if (!isFirst)
                    {
                        dup2(fd_prev[0], STDIN_FILENO);
                        close(fd_prev[0]);
                        close(fd_prev[1]);
                    }
                    execvp(command[0], command);
                }
                else
                {
                    procCount++;
                    if (!isFirst)
                    {
                        close(fd_prev[0]);
                        close(fd_prev[1]);
                    }
                    else
                    {
                        isFirst = false;
                    }
                    if (buffer != NULL)
                    {
                        fd_prev[0] = fd_curr[0];
                        fd_prev[1] = fd_curr[1];
                        pipe(fd_curr);
                    }   
                }
                
                command[argidx] = calloc(100, sizeof(char));
                strcpy(command[argidx], "prev");
                argidx = 0;
            }
            //////////////////////////////////////////////
        }
        for (int i = 0; i < procCount; i++)
        {
            wait(NULL);
        }
        filetok(file, line, "\n");
    }
    for (int i = 0; i < ARGS_LIMIT + 1; i++)
    {
        if (command[i] != NULL)
            free(command[i]);
    }
    fclose(file);
}

///////////// Definitions /////////////////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}

void filetok(FILE *file, char* dest, char *delims)
{
    char ch = getc(file);
    if (ch == EOF)
    {
        dest[0] = EOF;
        return;
    }
    int i = 0;
    int contFlag = 1;
    while (ch != EOF)
    {
        for (int j = 0; delims[j] != '\0'; j++)
        {
            if (ch == delims[j])
            {
                contFlag = 0;
                break;
            }
        }
        if (contFlag == 0)
        {
            break;
        }
        dest[i] = ch;
        ch = getc(file);
        i++;
    }
    dest[i] = '\0';
}