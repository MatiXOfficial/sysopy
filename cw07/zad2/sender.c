#include "common_header.h"

void handleSIGINT();

sem_t *semPackedAccess;
sem_t *semPackedFreeSpace;
sem_t *semPackedCount;

int shmPacked;
int *packedArr;
int shmArrInfo;
struct ArrInfo *arrInfo;

int main()
{
    semPackedAccess = sem_open(SEM_PACKED_ACCESS_NAME, O_RDWR, 0777, 0);
    semPackedFreeSpace = sem_open(SEM_PACKED_FREE_SPACE_NAME, O_RDWR, 0777, 0);
    semPackedCount = sem_open(SEM_PACKED_COUNT_NAME, O_RDWR, 0777, 0);

    shmPacked = shm_open(SHM_PACKED_NAME, O_RDWR, 0777);
    packedArr = mmap(NULL, ARR_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmPacked, 0);
    shmArrInfo = shm_open(SHM_ARR_INFO_NAME, O_RDWR, 0777);
    arrInfo = mmap(NULL, sizeof(struct ArrInfo), PROT_READ | PROT_WRITE, MAP_SHARED, shmArrInfo, 0);

    char timestamp[512];

    while(true)
    {
        sleep(SENDER_SLEEP);

        sem_wait(semPackedCount); // Requesting decrementation of packedArr elements count
        sem_post(semPackedFreeSpace); // Requesting incrementation of free space in packedArr
        sem_wait(semPackedAccess); // Requesting access to packedArr

        int orderSize = packedArr[arrInfo->packedFirst] * 3;
        arrInfo->packedFirst = (arrInfo->packedFirst + 1) % ARR_SIZE;
        arrInfo->packedCount--;

        getTimeStamp(timestamp);
        printf("(%d %s) Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
               getpid(), timestamp, orderSize, arrInfo->receivedCount, arrInfo->packedCount);

        sem_post(semPackedAccess);   // Releasing access to receivedArr
    }
}

void handleSIGINT()
{
    sem_close(semPackedAccess);
    sem_close(semPackedFreeSpace);
    sem_close(semPackedCount);

    munmap(packedArr, ARR_SIZE * sizeof(int));
    munmap(arrInfo, sizeof(struct ArrInfo));

    exit(0);
}