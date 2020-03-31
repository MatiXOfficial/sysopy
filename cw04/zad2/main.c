#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const int SIGNAL = SIGUSR1;

void error(char *mes);
void handleSig(int signum);
void checkPending(int signum);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("Wrong arguments number! Use ./main [ignore/handler/mask/pending]");
    }
    int opt;
    if (strcmp(argv[1], "ignore") == 0)
        opt = 0;
    else if (strcmp(argv[1], "handler") == 0)
        opt = 1;
    else if (strcmp(argv[1], "mask") == 0)
        opt = 2;
    else if (strcmp(argv[1], "pending") == 0)
        opt = 3;
    else
        error("Wrong argument! Use ./main [ignore/handler/mask/pending]");

    printf("Wybrano opcje: %s\n", argv[1]);

    if (opt == 0)
    {
        signal(SIGNAL, SIG_IGN);
    }
    else if (opt == 1)
    {
        signal(SIGNAL, handleSig);
    }
    else if (opt == 2 || opt == 3)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGNAL);
        sigprocmask(SIG_BLOCK, &mask, NULL);
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }

    ///////////// Przodek /////////////////////
    printf("Na początku, w procesie przodka:\n");
    raise(SIGNAL);

    if (opt == 3)
    {
        checkPending(SIGNAL);
    }

    ///////////// Dziecko po forku /////////////
    pid_t childPid = fork();
    if (childPid == 0)
    {
        printf("Po forku, w procesie potomnym:\n");
        if (opt != 3)
        {
            raise(SIGNAL);
        }
        else
        {
            checkPending(SIGNAL);
        }
        exit(0);
    }
    else
    {
        wait(NULL);
    }

    //////////// Funkcja exec //////////////////
    if (opt != 1)
    {
        char sigBuff[8];
        sprintf(sigBuff, "%d", SIGNAL);
        char optBuff[2];
        sprintf(optBuff, "%d", opt);
        execl("exec_fun", "exec_fun", sigBuff, optBuff, NULL);
    }
}


///////////////// Definicje ////////////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}

void handleSig(int signum)
{
    printf("->Zarejestrowano sygnal %d.\n", signum);
}

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