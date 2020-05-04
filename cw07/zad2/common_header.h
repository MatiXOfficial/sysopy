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
#include <sys/types.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define ARR_SIZE 4

#define ORDER_SIZE_MIN 1
#define ORDER_SIZE_MAX 10

#define RECEIVER_SLEEP 3
#define PACKER_SLEEP 2
#define SENDER_SLEEP 1

#define SEM_RECEIVED_ACCESS_NAME "/0"
#define SEM_RECEIVED_FREE_SPACE_NAME "/1"
#define SEM_RECEIVED_COUNT_NAME "/2"
#define SEM_PACKED_ACCESS_NAME "/3"
#define SEM_PACKED_FREE_SPACE_NAME "/4"
#define SEM_PACKED_COUNT_NAME "/5"

#define SHM_RECEIVED_NAME "/6"
#define SHM_PACKED_NAME "/7"
#define SHM_ARR_INFO_NAME "/8"

void getTimeStamp(char timestamp[512]);
void error(char *mes);

struct ArrInfo
{
    int receivedFirst;
    int receivedCount;

    int packedFirst;
    int packedCount;
};


#include "common_header.c"