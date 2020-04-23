#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_LENGTH 2048   // Max message length
#define SERVER_PROJ_ID 0
#define MAX_CLIENTS 20 // Max clients number that can be handled by the server

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
    bool isConnected;
    pid_t pid;
};
int currentClientProjID = 1;

struct Message
{
    long mtype;
    int info1;
    int info2;
    char text[MAX_LENGTH];
};
const int messageSize = 2 * sizeof(int) + sizeof(char[MAX_LENGTH]);

void error(char *mes);

#include "common_header.c"