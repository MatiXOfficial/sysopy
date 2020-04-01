#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void error(char *mes);

void handleSIGCHLD(int signum, siginfo_t *siginfo, void *ucontext);
void handleSIGFPE(int signum, siginfo_t *siginfo, void *ucontext);
void handleSIGSEGV(int signum, siginfo_t *siginfo, void *ucontext);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error("Wrong arguments number! Use ./main [SIGCHLD/SIGFPE/SIGSEGV]");
    }

    /*
    SIGCHLD - finished child process
    SIGFPE - floating point exception - division by 0
    SIGSEGV - segmentation fault
    */

    struct sigaction act;
    sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;

    if (strcmp(argv[1], "SIGCHLD") == 0)
    {
        act.sa_sigaction = handleSIGCHLD;
        sigaction(SIGCHLD, &act, NULL);
        if (fork() == 0)
            exit(17);
        else
            wait(NULL);
    }
    else if (strcmp(argv[1], "SIGFPE") == 0)
    {
        act.sa_sigaction = handleSIGFPE;
        sigaction(SIGFPE, &act, NULL);
        int a = 5 / 0;
    }
    else if (strcmp(argv[1], "SIGSEGV") == 0)
    {
        act.sa_sigaction = handleSIGSEGV;
        sigaction(SIGSEGV, &act, NULL);
        int *a = NULL;
        a[10] = 15;
    }
    else
    {
        error("Wrong argument! Use ./main [SIGCHLD/SIGFPE/SIGSEGV]");
    }
}

///////////////// Definicje ////////////////////////
void error(char *mes)
{
    perror(mes);
    exit(1);
}


void handleSIGCHLD(int signum, siginfo_t *siginfo, void *ucontext)
{
    printf("\nSIGCHLD\n");
    printf("Exit value - %d\n", siginfo->si_status);
}

void handleSIGFPE(int signum, siginfo_t *siginfo, void *ucontext)
{
    printf("\nSIGFPE\n");
    printf("Signal code - %d\n", siginfo->si_code);
    exit(0);
}

void handleSIGSEGV(int signum, siginfo_t *siginfo, void *ucontext)
{
    printf("\nSIGSEGV\n");
    printf("Address of faulting instruction - %p\n", siginfo->si_addr);
    exit(0);
}