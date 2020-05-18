#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>

#include "utils.h"

struct WaitingRoom
{
    int *queue;
    int first;
    int size;
    int maxSize;
    int latest;
};

void *barberFun(void *ptr);
void *clientFun(void *ptr);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condBarber = PTHREAD_COND_INITIALIZER;
pthread_cond_t condFinished = PTHREAD_COND_INITIALIZER;

struct WaitingRoom wr;
int isBarberAsleep = 0;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        error("Wrong number of arguments. Try ./main [chairs] [clients]");
    }
    int nChairs = atoi(argv[1]);
    int nClients = atoi(argv[2]);
    srand(time(NULL));

    wr.queue = calloc(nChairs, sizeof(int));
    wr.first = 0;
    wr.size = 0;
    wr.maxSize = nChairs;
    wr.latest = -1;
    
    pthread_t barberID;
    if (pthread_create(&barberID, NULL, barberFun, NULL) == -1)
    {
        error("could not create barber thread");
    }

    char buffer[128];
    pthread_t *clientIds = calloc(nClients, sizeof(pthread_t));
    for (int i = 0; i < nClients; i++)
    {
        sleepRandomSec(1, 2);
        int *id = calloc(1, sizeof(int));
        *id = i;
        if (pthread_create(&clientIds[i], NULL, clientFun, (void *) id) == -1)
        {
            sprintf(buffer, "could not create client no: %d", i);
            error(buffer);
        }
    }

    pthread_join(barberID, NULL);
    for (int i = 0; i < nClients; i++)
    {
        pthread_join(clientIds[i], NULL);
    }

    free(wr.queue);
    free(clientIds);

    pthread_cond_destroy(&condBarber);
    pthread_cond_destroy(&condFinished);
    pthread_mutex_destroy(&mutex);
}


////////////// Definitions ////////////
void *barberFun(void *ptr)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if (wr.size == 0)
        {
            printf("Golibroda: ide spac.\n");
            isBarberAsleep = 1;
            pthread_cond_wait(&condBarber, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        isBarberAsleep = 0;

        pthread_mutex_lock(&mutex);
        wr.latest = wr.queue[wr.first];
        wr.first = (wr.first + 1) % wr.maxSize;
        wr.size--;
        pthread_mutex_unlock(&mutex);

        printf("Golibroda: czeka %d klientow, gole klienta %d\n", wr.size, wr.latest);
        sleepRandomSec(SLEEP_MIN, SLEEP_MAX);
        pthread_cond_broadcast(&condFinished);
    }

    pthread_exit(NULL);
}

void *clientFun(void *ptr)
{
    int id = *((int *) ptr);
    free(ptr);

    while(1)
    {
        pthread_mutex_lock(&mutex);
        if (wr.size == wr.maxSize)
        {
            printf("Zajete; %d\n", id);
        }
        else
        {
            if (isBarberAsleep == 1)
            {
                printf("Budze golibrode; %d\n", id);
                pthread_cond_broadcast(&condBarber);
            }
            wr.queue[(wr.first + wr.size) % wr.maxSize] = id;
            wr.size++;
            printf("Poczekalnia, wolne miejsca: %d; %d\n", wr.maxSize - wr.size, id);
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        sleepRandomSec(SLEEP_MIN, SLEEP_MAX);
    }

    pthread_mutex_lock(&mutex);
    while (wr.latest != id)
    {
        pthread_cond_wait(&condFinished, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}