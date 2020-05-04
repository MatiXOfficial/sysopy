#include "common_header.h"

void handleSIGINT();

sem_t *semReceivedAccess;
sem_t *semReceivedFreeSpace;
sem_t *semReceivedCount;
sem_t *semPackedAccess;
sem_t *semPackedFreeSpace;
sem_t *semPackedCount;

int shmReceived;
int *receivedArr;
int shmPacked;
int *packedArr;
int shmArrInfo;
struct ArrInfo *arrInfo;

int main()
{
    semReceivedAccess = sem_open(SEM_RECEIVED_ACCESS_NAME, O_RDWR, 0777, 0);
    semReceivedFreeSpace = sem_open(SEM_RECEIVED_FREE_SPACE_NAME, O_RDWR, 0777, 0);
    semReceivedCount = sem_open(SEM_RECEIVED_COUNT_NAME, O_RDWR, 0777, 0);
    semPackedAccess = sem_open(SEM_PACKED_ACCESS_NAME, O_RDWR, 0777, 0);
    semPackedFreeSpace = sem_open(SEM_PACKED_FREE_SPACE_NAME, O_RDWR, 0777, 0);
    semPackedCount = sem_open(SEM_PACKED_COUNT_NAME, O_RDWR, 0777, 0);

    shmReceived = shm_open(SHM_RECEIVED_NAME, O_RDWR, 0777);
    receivedArr = mmap(NULL, ARR_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmReceived, 0);
    shmPacked = shm_open(SHM_PACKED_NAME, O_RDWR, 0777);
    packedArr = mmap(NULL, ARR_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmPacked, 0);
    shmArrInfo = shm_open(SHM_ARR_INFO_NAME, O_RDWR, 0777);
    arrInfo = mmap(NULL, sizeof(struct ArrInfo), PROT_READ | PROT_WRITE, MAP_SHARED, shmArrInfo, 0);

    char timestamp[512];

    while(true)
    {
        sleep(PACKER_SLEEP);

        sem_wait(semReceivedCount); // Requesting decrementation of receivedArr elements count
        sem_post(semReceivedFreeSpace);  // Requesting incrementation of free space in receivedArr
        sem_wait(semReceivedAccess); // Requesting access to receivedArr

        int orderSize = receivedArr[arrInfo->receivedFirst] * 2;
        arrInfo->receivedFirst = (arrInfo->receivedFirst + 1) % ARR_SIZE;
        arrInfo->receivedCount--;

        sem_post(semReceivedAccess); // Releasing access to receivedArr

        sem_wait(semPackedFreeSpace); // Requesting decrementation of free space in packedArr
        sem_post(semPackedCount); // Requesting incrementation of packedArr elements count
        sem_wait(semPackedAccess); // Requesting access to packedArr

        packedArr[(arrInfo->packedFirst + arrInfo->packedCount) % ARR_SIZE] = orderSize;
        arrInfo->packedCount++;

        getTimeStamp(timestamp);
        printf("(%d %s) Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
               getpid(), timestamp, orderSize, arrInfo->receivedCount, arrInfo->packedCount);

        sem_post(semPackedAccess);   // Releasing access to receivedArr
    }
}

void handleSIGINT()
{
    sem_close(semReceivedAccess);
    sem_close(semReceivedFreeSpace);
    sem_close(semReceivedCount);
    sem_close(semPackedAccess);
    sem_close(semPackedFreeSpace);
    sem_close(semPackedCount);

    munmap(receivedArr, ARR_SIZE * sizeof(int));
    munmap(packedArr, ARR_SIZE * sizeof(int));
    munmap(arrInfo, sizeof(struct ArrInfo));

    exit(0);
}