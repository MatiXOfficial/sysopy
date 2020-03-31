#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void checkPending(int signum);

int main(int argc, char *argv[])
{
    printf("Po wykonaniu funkcji exec:\n");
    int SIGNAL = atoi(argv[1]);
    int opt = atoi(argv[2]);
    if (opt != 3)
    {
        raise(SIGNAL);
    }
    else
    {
        checkPending(SIGNAL);
    }
}

////////////// Definicje ///////////////////////
void checkPending(int signum)
{
    sigset_t mask;
    sigpending(&mask);
    if (sigismember(&mask, signum))
    {
        printf("->Sygnal %d oczekuje.\n", signum);
    }
    else
    {
        printf("->Nie wykryto sygnalu oczekujacego (%d).\n", signum);
    }
}