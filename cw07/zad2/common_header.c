#include "common_header.h"

void getTimeStamp(char timestamp[512])
{
    struct timeval tv;
    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(timestamp, 512,"%m-%d-%Y %T", localtime(&curtime));
    sprintf(timestamp, "%s:%ld", timestamp, tv.tv_usec / 1000);
}

void error(char *mes)
{
    perror(mes);
    exit(1);
}