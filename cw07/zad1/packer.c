#include "common_header.h"

int main()
{
    int semID = createSem(semProjID, 0, NULL);

    int *receivedArr = shmat(createShm(shmReceivedProjID, 0), NULL, 0);
    int *packedArr = shmat(createShm(shmPackedProjID, 0), NULL, 0);
    struct ArrInfo *arrInfo = shmat(createShm(shmArrInfoProjID, 0), NULL, 0);

    char timestamp[512];

    while(true)
    {
        sleep(PACKER_SLEEP);

        struct sembuf ops[3];
        ops[0].sem_num = SEM_RECEIVED_COUNT; // Requesting decrementation of receivedArr elements count
        ops[0].sem_op = -1;
        ops[0].sem_flg = 0;
        ops[1].sem_num = SEM_RECEIVED_FREE_SPACE; // Requesting incrementation of free space in receivedArr
        ops[1].sem_op = 1;
        ops[1].sem_flg = 0;
        ops[2].sem_num = SEM_RECEIVED_ACCESS; // Requesting access to receivedArr
        ops[2].sem_op = -1;
        ops[2].sem_flg = 0;
        semop(semID, ops, 3);

        int orderSize = receivedArr[arrInfo->receivedFirst] * 2;
        arrInfo->receivedFirst = (arrInfo->receivedFirst + 1) % ARR_SIZE;
        arrInfo->receivedCount--;

        semOperation(semID, SEM_RECEIVED_ACCESS, 1);    // Releasing access to receivedArr

        ops[0].sem_num = SEM_PACKED_FREE_SPACE; // Requesting decrementation of free space in packedArr
        ops[0].sem_op = -1;
        ops[0].sem_flg = 0;
        ops[1].sem_num = SEM_PACKED_COUNT; // Requesting incrementation of packedArr elements count
        ops[1].sem_op = 1;
        ops[1].sem_flg = 0;
        ops[2].sem_num = SEM_PACKED_ACCESS; // Requesting access to packedArr
        ops[2].sem_op = -1;
        ops[2].sem_flg = 0;
        semop(semID, ops, 3);

        packedArr[(arrInfo->packedFirst + arrInfo->packedCount) % ARR_SIZE] = orderSize;
        arrInfo->packedCount++;

        getTimeStamp(timestamp);
        printf("(%d %s) Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
               getpid(), timestamp, orderSize, arrInfo->receivedCount, arrInfo->packedCount);

        semOperation(semID, SEM_PACKED_ACCESS, 1);    // Releasing access to receivedArr
    }
}