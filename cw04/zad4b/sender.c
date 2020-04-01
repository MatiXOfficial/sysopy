#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum Option
{
    KILL = 0,
    SIGQUEUE = 1,
    SIGRT = 2
};

void error(char *mes);
void handleSIGCOUNT(int signum, siginfo_t *siginfo, void *ucontext);
void handleSIGEND(int signum, siginfo_t *siginfo, void *ucontext);

int SIGCOUNT = SIGUSR1;
int SIGEND = SIGUSR2;

int waitingForSIGEND = 1;
int receivedCounter = -1;
enum Option opt;
pid_t catcherPid;
int waitForConfirmation;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        error("Wrong arguments number! Try ./sender [catcher pid] [sig count] [kill/sigqueue/sigrt]");
    }
    catcherPid = atoi(argv[1]);
    int sendCount = atoi(argv[2]);
    if (strcmp(argv[3], "kill") == 0)
    {
        opt = KILL;
    }
    else if (strcmp(argv[3], "sigqueue") == 0)
    {
        opt = SIGQUEUE;
    }
    else if (strcmp(argv[3], "sigrt") == 0)
    {
        opt = SIGRT;
        SIGCOUNT = SIGRTMIN;
        SIGEND = SIGRTMIN + 1;
    }
    else
    {
        error("Wrong argument! Try ./sender [catcher pid] [sig count] [kill/sigqueue/sigrt]");
    }

    //////////////// Receiving signals (later) //////////////
    struct sigaction actSIGCOUNT;
    sigemptyset(&actSIGCOUNT.sa_mask);
    actSIGCOUNT.sa_flags = SA_SIGINFO;
    actSIGCOUNT.sa_sigaction = handleSIGCOUNT;
    sigaction(SIGCOUNT, &actSIGCOUNT, NULL);

    struct sigaction actSIGEND;
    sigemptyset(&actSIGEND.sa_mask); 
    actSIGEND.sa_flags = SA_SIGINFO;
    actSIGEND.sa_sigaction = handleSIGEND;
    sigaction(SIGEND, &actSIGEND, NULL);

    ///////////////// Blocking signals //////////////////
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGCOUNT);
    sigdelset(&mask, SIGEND);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        error("Could not block the signals!");
    }
        
    ///////////////// Sending signals ///////////////////
    if (opt == KILL || opt == SIGRT)
    {
        for (int i = 0; i < sendCount; i++)
        {
            waitForConfirmation = 1;
            kill(catcherPid, SIGCOUNT);
            while(waitForConfirmation == 1)
            {
                sleep(1);
            }
        }
        kill(catcherPid, SIGEND);
    }
    else // if (opt == SIGQUEUE)
    {
        union sigval value;
        value.sival_int = 0;
        for (int i = 0; i < sendCount; i++)
        {
            waitForConfirmation = 1;
            sigqueue(catcherPid, SIGCOUNT, value);
            while(waitForConfirmation == 1)
            {
                sleep(1);
            }
        }
        sigqueue(catcherPid, SIGEND, value);
    }
    receivedCounter = 0;

    while(waitingForSIGEND == 1)
    {
        sleep(5);
    }
    printf("sender -> received: %d, sent: %d\n", receivedCounter, sendCount);
}

//////////////////////////////////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}

void handleSIGCOUNT(int signum, siginfo_t *siginfo, void *ucontext)
{
    if (receivedCounter != -1)  // receiving phase
    {
        receivedCounter++;
        if (opt == KILL || opt == SIGRT)
        {
            kill(catcherPid, SIGCOUNT);
        }
        else // if (opt == SIGQUEUE)
        {
            printf("sender received: %d, catcher sent: %d\n", receivedCounter, siginfo->si_value.sival_int);
            union sigval value;
            sigqueue(catcherPid, SIGCOUNT, value);
        }
    }
    else    // sending phase
    {
        waitForConfirmation = 0;
    }
}

void handleSIGEND(int signum, siginfo_t *siginfo, void *ucontext)
{
    waitingForSIGEND = 0;
}