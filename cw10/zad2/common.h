#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>

#define _BSD_SOURCE
#include <sys/socket.h>
#include <endian.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 8
#define MAX_NAME_LENGTH 32

void error(char *mes);


#include "common.c"