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
int receivedCounter = 0;
pid_t senderPid;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("Wrong arguments number! Try ./catcher [kill/sigqueue/sigrt]");
    }
    enum Option opt;
    if (strcmp(argv[1], "kill") == 0)
    {
        opt = KILL;
    }
    else if (strcmp(argv[1], "sigqueue") == 0)
    {
        opt = SIGQUEUE;
    }
    else if (strcmp(argv[1], "sigrt") == 0)
    {
        opt = SIGRT;
        SIGCOUNT = SIGRTMIN;
        SIGEND = SIGRTMIN + 1;
    }
    else
    {
        error("Wrong argument! Try ./catcher [kill/sigqueue/sigrt]");
    }
    printf("You chose %s.\n", argv[1]);
    printf("catcher -> pid: %d\n", getpid());

    ///////////////// Blocking signals //////////////////
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGCOUNT);
    sigdelset(&mask, SIGEND);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        error("Could not block the signals!");
    }

    //////////////// Receiving signals //////////////////
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

    while(waitingForSIGEND == 1)
    {
        sleep(5);
    }

    ///////////////// Sending signals ///////////////////
    if (opt == KILL || opt == SIGRT)
    {
        for (int i = 0; i < receivedCounter; i++)
        {
            kill(senderPid, SIGCOUNT);
        }
        kill(senderPid, SIGEND);
    }
    else // if (opt == SIGQUEUE)
    {
        union sigval value;
        for (int i = 0; i < receivedCounter; i++)
        {
            value.sival_int = i;
            sigqueue(senderPid, SIGCOUNT, value);
        }
        sigqueue(senderPid, SIGEND, value);
    }
    printf("catcher -> received: %d\n", receivedCounter);
}

//////////////////////////////////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}

void handleSIGCOUNT(int signum, siginfo_t *siginfo, void *ucontext)
{
    receivedCounter++;
}

void handleSIGEND(int signum, siginfo_t *siginfo, void *ucontext)
{
    senderPid = siginfo->si_pid;
    waitingForSIGEND = 0;
}