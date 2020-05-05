#include "common_header.h"

void handleSIGINT();

sem_t *semReceivedAccess;

int shmReceived;
int *receivedArr;
int shmArrInfo;
struct ArrInfo *arrInfo;

int main()
{
    semReceivedAccess = sem_open(SEM_RECEIVED_ACCESS_NAME, O_RDWR, 0777, 0);

    shmReceived = shm_open(SHM_RECEIVED_NAME, O_RDWR, 0777);
    receivedArr = mmap(NULL, ARR_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmReceived, 0);
    shmArrInfo = shm_open(SHM_ARR_INFO_NAME, O_RDWR, 0777);
    arrInfo = mmap(NULL, sizeof(struct ArrInfo), PROT_READ | PROT_WRITE, MAP_SHARED, shmArrInfo, 0);

    srand(time(NULL));
    char timestamp[512];
    int newOrderSize = 0;

    while(true)
    {
        sleep(RECEIVER_SLEEP);
        if (newOrderSize == 0)
            newOrderSize = rand() % (ORDER_SIZE_MAX - ORDER_SIZE_MIN) + ORDER_SIZE_MIN;

        sem_wait(semReceivedAccess); // Requesting access to receivedArr

        if (arrInfo->receivedCount != ARR_SIZE) // Not full array
        {
            receivedArr[(arrInfo->receivedFirst + arrInfo->receivedCount) % ARR_SIZE] = newOrderSize;
            arrInfo->receivedCount++;

            getTimeStamp(timestamp);
            printf("(%d %s) Dodalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
                getpid(), timestamp, newOrderSize, arrInfo->receivedCount, arrInfo->packedCount);
            newOrderSize = 0;
        }

        sem_post(semReceivedAccess); // Releasing access to receivedArr
    }
}

void handleSIGINT()
{
    sem_close(semReceivedAccess);

    munmap(receivedArr, ARR_SIZE * sizeof(int));
    munmap(arrInfo, sizeof(struct ArrInfo));

    exit(0);
}