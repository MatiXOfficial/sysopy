#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

#define ARR_SIZE 4

#define ORDER_SIZE_MIN 1
#define ORDER_SIZE_MAX 10

#define RECEIVER_SLEEP 3
#define PACKER_SLEEP 2
#define SENDER_SLEEP 1

#define semProjID 0
#define shmReceivedProjID 1
#define shmPackedProjID 2
#define shmArrInfoProjID 3

void getTimeStamp(char timestamp[512]);
void error(char *mes);
int createShm(int projID, int size);
int createSem(int projID, int nsems, int *init);
void semOperation(int semID, int semNum, int semOp);

struct ArrInfo
{
    int receivedFirst;
    int receivedCount;

    int packedFirst;
    int packedCount;
};

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

enum SemTypes
{
    SEM_RECEIVED_ACCESS = 0,
    SEM_RECEIVED_FREE_SPACE,
    SEM_RECEIVED_COUNT,
    SEM_PACKED_ACCESS,
    SEM_PACKED_FREE_SPACE,
    SEM_PACKED_COUNT
};


#include "common_header.c"