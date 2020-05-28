#include "common.h"

void error(char *mes)
{
    perror(mes);
    exit(1);
}