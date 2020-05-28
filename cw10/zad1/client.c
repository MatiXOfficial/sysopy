#include "common.h"

void exitFun();
void printGrid(char *state);

int fd_client;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        error("Wrong number of arguments! Try ./client [name] [inet/unix] [server address:port/path]");
    }
    char clientName[MAX_NAME_LENGTH];
    strcpy(clientName, argv[1]);

    if (strcmp(argv[2], "inet") == 0)
    {
        fd_client = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_client == -1)
            error("socket error");

        char *address = strtok(argv[3], ":");
        char *port = strtok(NULL, "\0");

        struct sockaddr_in addr_client = {.sin_family = AF_INET, .sin_port = htobe16(atoi(port))};
        if (inet_pton(AF_INET, address, &addr_client.sin_addr) == -1)
            error("inet_pton error");

        if (connect(fd_client, (struct sockaddr *)&addr_client, sizeof(addr_client)) == -1)
            error("connect error");
    }
    else if (strcmp(argv[2], "unix") == 0)
    {
        fd_client = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd_client == -1)
            error("socket error");

        struct sockaddr_un addr_client = {.sun_family = AF_UNIX};
        strcpy(addr_client.sun_path, argv[3]);

        if (connect(fd_client, (struct sockaddr *)&addr_client, sizeof(addr_client)) == -1)
            error("connect error");
    }
    else
    {
        error("Second argument is wrong! Try ./client [name] [inet/unix] [server port/path]");
    }
    signal(SIGINT, exitFun);

    int length = strlen(clientName) + 1;
    write(fd_client, &length, sizeof(int)); // send name
    write(fd_client, clientName, length);

    char state;
    read(fd_client, &state, 1);
    if (state == '1')
    {
        printf("Could not connect. Number of clients exceeded\n");
        raise(SIGINT);
    }
    if (state == '2')
    {
        printf("Could not connect. Name already in use.\n");
        raise(SIGINT);
    }

    printf("Connected succesfully. Waiting for other player.\n");

    char opponentName[MAX_NAME_LENGTH];
    read(fd_client, &length, sizeof(int)); // read opponent's name
    if (length == 'T')
    {
        printf("You have been disconnected (timeout).\n");
        raise(SIGINT);
    }
    read(fd_client, opponentName, length);
    printf("Started game with %s.\n", opponentName);

    char gridState[9];
    read(fd_client, gridState, 9); // read grid numbers
    printGrid(gridState);

    char sign;
    int isMyTurn;
    read(fd_client, &sign, 1); // read assigned symbol
    if (sign == 'X')
    {
        printf("Your symbol is X. You start! Waiting for your move: ");
        isMyTurn = 1;
    }
    else
    {
        printf("Your symbol is O. Wait for your opponent...\n");
        isMyTurn = 0;
    }

    while(1)
    {
        if (isMyTurn)
        {
            int choice;
            scanf("%d", &choice);
            while (choice < 0 || choice > 8)
            {
                printf("Wrong input. Type the number from 0 to 8: ");
                scanf("%d", &choice);
            }
            char sign[2];
            sprintf(sign, "%d", choice);
            write(fd_client, sign, 1);

            char result;
            read(fd_client, &result, 1);
            if (result == 'D')
            {
                printf("Your opponent has been disconnected.\n");
                break;
            }
            else if (result == 'T')
            {
                printf("You have been disconnected (timeout).\n");
                break;
            }
            else if (result == '3')
            {
                printf("Wrong input. This field is already chosen.\nWaiting for your move: ");
                continue;
            }
            char grid[9];
            read(fd_client, grid, 9);
            printGrid(grid);
            if (result == '0')
            {
                printf("Wait for your opponent...\n");
            }
            else if (result == '1')
            {
                printf("Congratulations! You won.\n");
                break;
            }
            else
            {
                printf("It's a draw :(\n");
                break;
            }
        }
        else
        {
            char sign;
            read(fd_client, &sign, 1);
            if (sign == 'D')
            {
                printf("Your opponent has been disconnected.\n");
                break;
            }
            else if (sign == 'T')
            {
                printf("You have been disconnected (timeout).\n");
                break;
            }
            char grid[9];
            read(fd_client, grid, 9);
            printGrid(grid);
            if (sign == '0')
            {
                printf("It's your turn. Waiting for your move: ");
            }
            else if (sign == '1')
            {
                printf("You lost :(\n");
                break;
            }
            else
            {
                printf("It's a draw :(\n");
                break;
            }
        }
        isMyTurn = (isMyTurn + 1) % 2;
    }

    raise(SIGINT);
}


///////////// Definitons ////////////////
void exitFun()
{
    if (shutdown(fd_client, SHUT_RDWR) == -1)
        error("shutdown error");
    if (close(fd_client) == -1)
        error("close error");

    exit(0);
}

void printGrid(char *state)
{
    printf("--------\n");
    printf("|%c|%c|%c|\n", state[0], state[1], state[2]);
    printf("--------\n");
    printf("|%c|%c|%c|\n", state[3], state[4], state[5]);
    printf("--------\n");
    printf("|%c|%c|%c|\n", state[6], state[7], state[8]);
    printf("--------\n");
}