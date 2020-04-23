#include "common_header.h"

int findFreeIdx();
void handleMessage(char message[MAX_LENGTH], int priority);

void handleStop(char message[MAX_LENGTH]);
void handleDisconnect(char message[MAX_LENGTH]);
void handleList(char message[MAX_LENGTH]);
void handleInit(char message[MAX_LENGTH]);
void handleConnect(char message[MAX_LENGTH]);

void handleSIGINT();

mqd_t serverQueueID;
struct Client **clientsArr;

int main()
{
    // Message queue creation
    serverQueueID = mq_open(SERVER_NAME, O_RDONLY | O_CREAT, 0777, NULL);
    if (serverQueueID == -1)
    {
        error("mq_open error");
    }

    clientsArr = calloc(MAX_CLIENTS, sizeof(struct Client *));

    printf("Server ready.\n");
    signal(SIGINT, handleSIGINT);
    char message[MAX_LENGTH];
    unsigned int priority;
    while(true)
    {
        if (mq_receive(serverQueueID, message, MAX_LENGTH, &priority) == -1)
        {
            error("mq_receive error");
        }
        handleMessage(message, priority);
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

void handleMessage(char message[MAX_LENGTH], int priority)
{
    switch(priority)
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

void handleStop(char message[MAX_LENGTH])
{
    int clientID = atoi(message);
    printf("Received STOP from %d.\n", clientID);
    mq_close(clientsArr[clientID]->queueID);
    free(clientsArr[clientID]);
    clientsArr[clientID] = NULL;
}

void handleDisconnect(char message[MAX_LENGTH])
{
    int clientID = atoi(message);
    printf("Receive DISCONNECT from %d\n", clientID);
    clientsArr[clientID]->isConnected = false;
}

void handleList(char message[MAX_LENGTH])
{
    int clientID = atoi(message);
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

    sprintf(message, "%02d%s", clientID, result);
    if (mq_send(clientQueueID, message, MAX_LENGTH, LIST) == -1)
    {
        error("mq_send error");
    }
}

void handleInit(char message[MAX_LENGTH])
{
    char *clientQueueName = strtok(message, ":");
    int clientQueueID = mq_open(clientQueueName, O_WRONLY);
    int pid = atoi(message + 10);
    int clientID = findFreeIdx();
    printf("Received INIT. New client: %d\n", clientID);
    if (clientID != -1)
    {
        clientsArr[clientID] = calloc(1, sizeof(struct Client));
        clientsArr[clientID]->queueID = clientQueueID;
        clientsArr[clientID]->pid = pid;
        clientsArr[clientID]->isConnected = false;
        strcpy(clientsArr[clientID]->queueName, clientQueueName);
    }

    sprintf(message, "%02d", clientID);
    if (mq_send(clientQueueID, message, MAX_LENGTH, INIT) == -1)
    {
        error("mq_send error");
    }
}

void handleConnect(char message[MAX_LENGTH])
{
    int client1ID = atoi(message);
    int client1QueueID = clientsArr[client1ID]->queueID;
    int client2ID = atoi(message + 2);
    printf("Received CONNECT from %d with %d.\n", client1ID, client2ID);
    if (clientsArr[client2ID] == NULL)
    {
        sprintf(message, "-1:Error: client %d does not exist", client2ID);
        if (mq_send(client1QueueID, message, MAX_LENGTH, CONNECT) == -1)
        {
            error("mq_send error");
        }
        return;
    }
    if (clientsArr[client1ID]->isConnected)
    {
        strcpy(message, "-1:Error: you are already connected.");
        if (mq_send(client1QueueID, message, MAX_LENGTH, CONNECT) == -1)
        {
            error("mq_send error");
        }
        return;
    }
    if (clientsArr[client2ID] == NULL)
    {
        sprintf(message, "-1:Error: client %d is already connected", client2ID);
        if (mq_send(client1QueueID, message, MAX_LENGTH, CONNECT) == -1)
        {
            error("mq_send error");
        }
        return;
    }

    sprintf(message, "%d:%s:Connected with %d.", client1ID, clientsArr[client2ID]->queueName, client2ID);
    if (mq_send(client1QueueID, message, MAX_LENGTH, CONNECT) == -1)
    {
        error("mq_send error");
    }
    if (mq_send(client1QueueID, message, MAX_LENGTH, CONNECT) == -1)
    {
        error("mq_send error");
    }

    usleep(100000);
    sprintf(message, "-1:%s:Connected with %d.", clientsArr[client1ID]->queueName, client1ID);
    if (mq_send(clientsArr[client2ID]->queueID, message, MAX_LENGTH, CONNECT) == -1)
    {
        error("mq_send error");
    }
    kill(clientsArr[client2ID]->pid, SIGUSR1);

    clientsArr[client1ID]->isConnected = true;
    clientsArr[client2ID]->isConnected = true;
}

void handleSIGINT()
{
    char message[MAX_LENGTH];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientsArr[i] != NULL)
        {
            kill(clientsArr[i]->pid, SIGINT);
            if (mq_receive(serverQueueID, message, MAX_LENGTH, NULL) == -1)
            {
                error("msgrcv error");
            }
            handleStop(message);
            free(clientsArr);
        }
    }
    printf("Server stops working.\n");
    mq_close(serverQueueID);
    mq_unlink(SERVER_NAME);
    free(clientsArr);
    exit(0);
}