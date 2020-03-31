#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handleSIGTSTP();
void handleSIGINT();

int isWaiting = 0;

int main()
{
    struct sigaction actSIGTSTP;
    actSIGTSTP.sa_handler = handleSIGTSTP;
    sigemptyset(&actSIGTSTP.sa_mask); 
    actSIGTSTP.sa_flags = 0;
    sigaction(SIGTSTP, &actSIGTSTP, NULL);

    signal(SIGINT, handleSIGINT);

    while(1)
    {
        if (isWaiting == 0)
        {
            system("ls");
        }
        sleep(1);
    }
}


//////////////////// Definicje /////////////////////////
void handleSIGTSTP()
{
    if (isWaiting == 0)
    {
        printf("\nOczekuje na CTRL + Z - kontynuacja albo CTRL + C - zakonczenie programu.\n");
    }
    else
    {
        printf("\n");
    }
    isWaiting = (isWaiting + 1) % 2;
    return;
}

void handleSIGINT()
{
    printf("\nOdebrano sygnal SIGINT.\n");
    exit(0);
}