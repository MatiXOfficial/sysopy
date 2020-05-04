#include "common_header.h"

void handleSIGINT();

sem_t *semReceivedAccess;
sem_t *semReceivedFreeSpace;
sem_t *semReceivedCount;

int shmReceived;
int *receivedArr;
int shmArrInfo;
struct ArrInfo *arrInfo;

int main()
{
    semReceivedAccess = sem_open(SEM_RECEIVED_ACCESS_NAME, O_RDWR, 0777, 0);
    semReceivedFreeSpace = sem_open(SEM_RECEIVED_FREE_SPACE_NAME, O_RDWR, 0777, 0);
    semReceivedCount = sem_open(SEM_RECEIVED_COUNT_NAME, O_RDWR, 0777, 0);

    shmReceived = shm_open(SHM_RECEIVED_NAME, O_RDWR, 0777);
    receivedArr = mmap(NULL, ARR_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmReceived, 0);
    shmArrInfo = shm_open(SHM_ARR_INFO_NAME, O_RDWR, 0777);
    arrInfo = mmap(NULL, sizeof(struct ArrInfo), PROT_READ | PROT_WRITE, MAP_SHARED, shmArrInfo, 0);

    srand(time(NULL));
    char timestamp[512];

    while(true)
    {
        sleep(RECEIVER_SLEEP);
        int newOrderSize = rand() % (ORDER_SIZE_MAX - ORDER_SIZE_MIN) + ORDER_SIZE_MIN;

        sem_wait(semReceivedFreeSpace); // Requesting decrementation of free space in receivedArr
        sem_post(semReceivedCount); // Requesting incrementation of receivedArr elements count
        sem_wait(semReceivedAccess); // Requesting access to receivedArr

        receivedArr[(arrInfo->receivedFirst + arrInfo->receivedCount) % ARR_SIZE] = newOrderSize;
        arrInfo->receivedCount++;

        getTimeStamp(timestamp);
        printf("(%d %s) Dodalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
               getpid(), timestamp, newOrderSize, arrInfo->receivedCount, arrInfo->packedCount);

        sem_post(semReceivedAccess); // Releasing access to receivedArr
    }
}

void handleSIGINT()
{
    sem_close(semReceivedAccess);
    sem_close(semReceivedFreeSpace);
    sem_close(semReceivedCount);

    munmap(receivedArr, ARR_SIZE * sizeof(int));
    munmap(arrInfo, sizeof(struct ArrInfo));

    exit(0);
}