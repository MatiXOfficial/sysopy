#include "common_header.h"

int main()
{
    int semID = createSem(semProjID, 0, NULL);

    int *receivedArr = shmat(createShm(shmReceivedProjID, 0), NULL, 0);
    struct ArrInfo *arrInfo = shmat(createShm(shmArrInfoProjID, 0), NULL, 0);

    srand(time(NULL));
    char timestamp[512];

    while(true)
    {
        sleep(RECEIVER_SLEEP);
        int newOrderSize = rand() % (ORDER_SIZE_MAX - ORDER_SIZE_MIN) + ORDER_SIZE_MIN;
        
        struct sembuf ops[3];
        ops[0].sem_num = SEM_RECEIVED_FREE_SPACE; // Requesting decrementation of free space in receivedArr
        ops[0].sem_op = -1;
        ops[0].sem_flg = 0;
        ops[1].sem_num = SEM_RECEIVED_COUNT; // Requesting incrementation of receivedArr elements count
        ops[1].sem_op = 1;
        ops[1].sem_flg = 0;
        ops[2].sem_num = SEM_RECEIVED_ACCESS; // Requesting access to receivedArr
        ops[2].sem_op = -1;
        ops[2].sem_flg = 0;
        semop(semID, ops, 3);

        receivedArr[(arrInfo->receivedFirst + arrInfo->receivedCount) % ARR_SIZE] = newOrderSize;
        arrInfo->receivedCount++;

        getTimeStamp(timestamp);
        printf("(%d %s) Dodalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", 
               getpid(), timestamp, newOrderSize, arrInfo->receivedCount, arrInfo->packedCount);

        semOperation(semID, SEM_RECEIVED_ACCESS, 1);    // Releasing access to receivedArr
    }
}