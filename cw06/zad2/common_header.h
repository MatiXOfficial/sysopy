#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           
#include <sys/stat.h>     
#include <mqueue.h>

#define MAX_LENGTH 8192   // Max message length
#define SERVER_NAME "/server"
#define CLIENT_NAME "/client"
#define MAX_CLIENTS 99 // Max clients number that can be handled by the server

enum Task
{
    STOP = 1,
    DISCONNECT,
    LIST,
    INIT,
    CONNECT
};

struct Client
{
    int queueID;
    char queueName[32];
    bool isConnected;
    pid_t pid;
};
int currentClientNameID = 1;

void error(char *mes);

#include "common_header.c"