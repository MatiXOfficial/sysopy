#include "common_header.h"

void getTimeStamp(char timestamp[512])
{
    struct timeval tv;
    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(timestamp, 512,"%m-%d-%Y %T", localtime(&curtime));
    sprintf(timestamp, "%s:%ld", timestamp, tv.tv_usec / 1000);
}

void error(char *mes)
{
    perror(mes);
    exit(1);
}

int createShm(int projID, int size)
{
    key_t key = ftok(getenv("HOME"), projID);
    if (key == -1)
    {
        error("ftok error");
    }

    int shmID;
    if (size == 0)
    {
        shmID = shmget(key, size, 0777);
    }
    else
    {
        shmID = shmget(key, size, IPC_CREAT | 0777);
    }
    if (shmID == -1)
    {
        error("shmget error");
    }
    return shmID;
}

int createSem(int projID, int nsems, int *initVals)
{
    key_t key = ftok(getenv("HOME"), projID);
    if (key == -1)
    {
        error("ftok error");
    }

    int semID;
    if (nsems == 0)
    {
        semID = semget(key, nsems, 0777);
    }
    else
    {
        semID = semget(key, nsems, IPC_CREAT | 0777);
    }
    if (semID == -1)
    {
        error("semget error");
    }

    if (nsems != 0)
    {
        union semun arg;
        for (int i = 0; i < nsems; i++)
        {
            arg.val = initVals[i];
            if (semctl(semID, i, SETVAL, arg) == -1)
            {
                error("semctl error");
            }
        }
    }

    return semID;
}

void semOperation(int semID, int semNum, int semOp)
{
    struct sembuf op;
    op.sem_num = semNum;
    op.sem_op = semOp;
    op.sem_flg = 0;
    
    if (semop(semID, &op, 1) == -1)
    {
        error("semop error");
    }
}