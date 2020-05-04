#include "common_header.h"

void handleSIGINT();

int allCount;
pid_t *pidArr;

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

    int initArr[] = {1, ARR_SIZE, 0, 1, ARR_SIZE, 0};
    int semID = createSem(semProjID, 6, initArr);

    // Shared memory
    int shmReceivedID = createShm(shmReceivedProjID, ARR_SIZE * sizeof(int));
    int shmPackedID = createShm(shmPackedProjID, ARR_SIZE * sizeof(int));
    int shmArrInfoID = createShm(shmArrInfoProjID, sizeof(struct ArrInfo));

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

    // Cleaning
    free(pidArr);
    semctl(semID, 0, IPC_RMID, NULL);
    shmctl(shmReceivedID, IPC_RMID, NULL);
    shmctl(shmPackedID, IPC_RMID, NULL);
    shmctl(shmArrInfoID, IPC_RMID, NULL);
}

void handleSIGINT()
{
    for (int i = 0; i < allCount; i++)
    {
        kill(pidArr[i], SIGINT);
    }
}