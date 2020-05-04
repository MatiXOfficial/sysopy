#include "common_header.h"

void handleSIGINT();

int allCount;
pid_t *pidArr;

sem_t *semReceivedAccess;
sem_t *semReceivedFreeSpace;
sem_t *semReceivedCount;
sem_t *semPackedAccess;
sem_t *semPackedFreeSpace;
sem_t *semPackedCount;

int main(int argc, char *argv[])
{
    signal(SIGINT, handleSIGINT);
    if (argc != 4)
    {
        error("Wrong number of arguments. Try .\\supervisor [receiverCount] [packerCount] [senderCount]");
    }
    int receiverCount = atoi(argv[1]);
    int packerCount = atoi(argv[2]);
    int senderCount = atoi(argv[3]);
    allCount = receiverCount + packerCount + senderCount;

    // Semaphores
    semReceivedAccess = sem_open(SEM_RECEIVED_ACCESS_NAME, O_RDWR | O_CREAT, 0777, 1);
    semReceivedFreeSpace = sem_open(SEM_RECEIVED_FREE_SPACE_NAME, O_RDWR | O_CREAT, 0777, ARR_SIZE);
    semReceivedCount = sem_open(SEM_RECEIVED_COUNT_NAME, O_RDWR | O_CREAT, 0777, 0);
    semPackedAccess = sem_open(SEM_PACKED_ACCESS_NAME, O_RDWR | O_CREAT, 0777, 1);
    semPackedFreeSpace = sem_open(SEM_PACKED_FREE_SPACE_NAME, O_RDWR | O_CREAT, 0777, ARR_SIZE);
    semPackedCount = sem_open(SEM_PACKED_COUNT_NAME, O_RDWR | O_CREAT, 0777, 0);

    // Shared memory
    int shmReceived = shm_open(SHM_RECEIVED_NAME, O_RDWR | O_CREAT, 0777);
    ftruncate(shmReceived, ARR_SIZE * sizeof(int));
    int shmPacked = shm_open(SHM_PACKED_NAME, O_RDWR | O_CREAT, 0777);
    ftruncate(shmPacked, ARR_SIZE * sizeof(int));
    int shmArrInfo = shm_open(SHM_ARR_INFO_NAME, O_RDWR | O_CREAT, 0777);
    ftruncate(shmArrInfo, sizeof(struct ArrInfo));

    pidArr = calloc(allCount, sizeof(pid_t));
    int pidIdx = 0;
    pid_t pid;

    for (int i = 0; i < receiverCount; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            execlp("./receiver", "receiver", NULL);
            error("execlp error");
        }
        else
        {
            pidArr[pidIdx] = pid;
            pidIdx++;
        }
    }
    for (int i = 0; i < packerCount; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            execlp("./packer", "packer", NULL);
            error("execlp error");
        }
        else
        {
            pidArr[pidIdx] = pid;
            pidIdx++;
        }
    }
    for (int i = 0; i < senderCount; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            execlp("./sender", "sender", NULL);
            error("execlp error");
        }
        else
        {
            pidArr[pidIdx] = pid;
            pidIdx++;
        }
    }

    for (int i = 0; i < allCount; i++)
    {
        wait(NULL);
    }
}

void handleSIGINT()
{
    for (int i = 0; i < allCount; i++)
    {
        kill(pidArr[i], SIGINT);
    }

    // Cleaning
    free(pidArr);

    sem_close(semReceivedAccess);
    sem_close(semReceivedFreeSpace);
    sem_close(semReceivedCount);
    sem_close(semPackedAccess);
    sem_close(semPackedFreeSpace);
    sem_close(semPackedCount);

    sem_unlink(SEM_RECEIVED_ACCESS_NAME);
    sem_unlink(SEM_RECEIVED_FREE_SPACE_NAME);
    sem_unlink(SEM_RECEIVED_COUNT_NAME);
    sem_unlink(SEM_PACKED_ACCESS_NAME);
    sem_unlink(SEM_PACKED_FREE_SPACE_NAME);
    sem_unlink(SEM_PACKED_COUNT_NAME);

    shm_unlink(SHM_RECEIVED_NAME);
    shm_unlink(SHM_PACKED_NAME);
    shm_unlink(SHM_ARR_INFO_NAME);

    exit(0);
}