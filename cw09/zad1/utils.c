#include "utils.h"

void error(char *mes)
{
    perror(mes);
    exit(1);
}

void sleepRandomSec(int min, int max)
{
    int howLong = rand() % (max - min) + min;
    sleep(howLong);
}