#include "common_header.h"

int findFreeIdx();
void handleMessage(struct Message message);

void handleStop(struct Message message);
void handleDisconnect(struct Message message);
void handleList(struct Message message);
void handleInit(struct Message message);
void handleConnect(struct Message message);

void handleSIGINT();

int serverQueueID;
struct Client **clientsArr;

int main()
{
    // Message queue creation
    key_t key = ftok(getenv("HOME"), SERVER_PROJ_ID);
    if (key == -1)
    {
        error("ftok error");
    }
    serverQueueID = msgget(key, IPC_CREAT | 0777);
    if (serverQueueID == -1)
    {
        error("msgget error");
    }

    clientsArr = calloc(MAX_CLIENTS, sizeof(struct Client *));

    printf("Server ready.\n");
    struct Message message;
    signal(SIGINT, handleSIGINT);
    while(true)
    {
        if (msgrcv(serverQueueID, &message, messageSize, -6, 0) == -1)
        {
            error("msgrcv error");
        }
        handleMessage(message);
    }
}

//////////////// Definitions /////////////////////
int findFreeIdx()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientsArr[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

void handleMessage(struct Message message)
{
    switch(message.mtype)
    {
        case STOP:
        {
            handleStop(message);
            break;
        }
        case DISCONNECT:
        {
            handleDisconnect(message);
            break;
        }
        case LIST:
        {
            handleList(message);
            break;
        }
        case INIT:
        {
            handleInit(message);
            break;
        }
        case CONNECT:
        {
            handleConnect(message);
            break;
        }
    }
}

void handleStop(struct Message message)
{
    int clientID = message.info1;
    printf("Received STOP from %d.\n", clientID);
    free(clientsArr[clientID]);
    clientsArr[clientID] = NULL;
}

void handleDisconnect(struct Message message)
{
    int clientID = message.info1;
     printf("Receive DISCONNECT from %d\n", clientID);
    clientsArr[clientID]->isConnected = false;
}

void handleList(struct Message message)
{
    int clientID = message.info1;
    printf("Received LIST from %d.\n", clientID);
    int clientQueueID = clientsArr[clientID]->queueID;

    char result[MAX_LENGTH];
    strcpy(result, "List of all clients:\n");

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientsArr[i] != NULL)
        {
            if (clientID == i)
            {
                sprintf(result, "%s-> ", result);
            }
            if (clientsArr[i]->isConnected)
            {
                sprintf(result, "%s%d - already connected\n", result, i);
            }
            else
            {
                sprintf(result, "%s%d - ready\n", result, i);
            }
        }
    }

    message.mtype = LIST;
    message.info1 = clientID;
    strcpy(message.text, result);
    if (msgsnd(clientQueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }
}

void handleInit(struct Message message)
{
    int clientQueueID = message.info1;
    int pid = message.info2;
    int clientID = findFreeIdx();
    printf("Received INIT. New client: %d\n", clientID);
    if (clientID != -1)
    {
        clientsArr[clientID] = calloc(1, sizeof(struct Client));
        clientsArr[clientID]->queueID = clientQueueID;
        clientsArr[clientID]->pid = pid;
        clientsArr[clientID]->isConnected = false;
    }

    message.mtype = INIT;
    message.info1 = clientID;
    if (msgsnd(clientQueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }
}

void handleConnect(struct Message message)
{
    int client1ID = message.info1;
    printf("Received CONNECT from %d.\n", client1ID);
    int client1QueueID = clientsArr[client1ID]->queueID;
    int client2ID = message.info2;
    if (clientsArr[client2ID] == NULL)
    {
        message.info1 = -1;
        sprintf(message.text, "Error: client %d does not exist", client2ID);
        if (msgsnd(client1QueueID, &message, messageSize, 0) == -1)
        {
            error("msgsnd error");
        }
        return;
    }
    if (clientsArr[client1ID]->isConnected)
    {
        message.info1 = -1;
        strcpy(message.text, "Error: you are already connected.");
        if (msgsnd(client1QueueID, &message, messageSize, 0) == -1)
        {
            error("msgsnd error");
        }
        return;
    }
    if (clientsArr[client2ID] == NULL)
    {
        message.info1 = -1;
        sprintf(message.text, "Error: client %d is already connected", client2ID);
        if (msgsnd(client1QueueID, &message, messageSize, 0) == -1)
        {
            error("msgsnd error");
        }
        return;
    }
    int client2QueueID = clientsArr[client2ID]->queueID;

    message.info1 = client1ID;
    message.info2 = client2QueueID;
    sprintf(message.text, "Connected with %d.", client2ID);
    if (msgsnd(client1QueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }
    if (msgsnd(client1QueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }

    usleep(100000);
    message.info1 = -1;
    message.info2 = client1QueueID;
    sprintf(message.text, "Connected with %d.", client1ID);
    if (msgsnd(client2QueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }
    kill(clientsArr[client2ID]->pid, SIGUSR1);

    clientsArr[client1ID]->isConnected = true;
    clientsArr[client2ID]->isConnected = true;
}

void handleSIGINT()
{
    struct Message message;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientsArr[i] != NULL)
        {
            kill(clientsArr[i]->pid, SIGINT);
            if (msgrcv(serverQueueID, &message, messageSize, -6, 0) == -1)
            {
                error("msgrcv error");
            }
            handleStop(message);
            free(clientsArr);
        }
    }
    printf("Server stops working.\n");
    msgctl(serverQueueID, IPC_RMID, 0);
    free(clientsArr);
    exit(0);
}