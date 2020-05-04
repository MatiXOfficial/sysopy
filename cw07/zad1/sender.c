#include "common_header.h"

int main()
{
    int semID = createSem(semProjID, 0, NULL);

    int *packedArr = shmat(createShm(shmPackedProjID, 0), NULL, 0);
    struct ArrInfo *arrInfo = shmat(createShm(shmArrInfoProjID, 0), NULL, 0);

    char timestamp[512];

    while(true)
    {
        sleep(SENDER_SLEEP);

        struct sembuf ops[3];
        ops[0].sem_num = SEM_PACKED_COUNT; // Requesting decrementation of packedArr elements count
        ops[0].sem_op = -1;
        ops[0].sem_flg = 0;
        ops[1].sem_num = SEM_PACKED_FREE_SPACE; // Requesting incrementation of free space in packedArr
        ops[1].sem_op = 1;
        ops[1].sem_flg = 0;
        ops[2].sem_num = SEM_PACKED_ACCESS; // Requesting access to packedArr
        ops[2].sem_op = -1;
        ops[2].sem_flg = 0;
        semop(semID, ops, 3);

        int orderSize = packedArr[arrInfo->packedFirst] * 3;
        arrInfo->packedFirst = (arrInfo->packedFirst + 1) % ARR_SIZE;
        arrInfo->packedCount--;

        getTimeStamp(timestamp);
        printf("(%d %s) Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
               getpid(), timestamp, orderSize, arrInfo->receivedCount, arrInfo->packedCount);

        semOperation(semID, SEM_PACKED_ACCESS, 1);    // Releasing access to receivedArr
    }
}