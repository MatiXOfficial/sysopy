#include "common_header.h"

void handleList();
void handleStop();
void handleConnect();

int serverQueueID;
int clientQueueID;
char clientQueueName[32];
int clientID;
int friendQueueId = -1;
char friendQueueName[32];

int main()
{
    // Server queue
    serverQueueID = mq_open(SERVER_NAME, O_WRONLY);
    if (serverQueueID == -1)
    {
        error("mq_open error (server queue");
    }

    // Client queue
    sprintf(clientQueueName, "%s%02d", SERVER_NAME, currentClientNameID++);
    clientQueueID = mq_open(clientQueueName, O_RDONLY | O_CREAT, 0777, NULL);
    if (clientQueueID == -1)
    {
        error("mq_open error (client queue)");
    }

    signal(SIGINT, handleStop);

    char message[MAX_LENGTH];
    sprintf(message, "%s:%010d", clientQueueName, getpid());
    if (mq_send(serverQueueID, message, strlen(message) + 1, INIT) == -1)
    {
        error("mq_send error");
    }

    if (mq_receive(clientQueueID, message, MAX_LENGTH, NULL) == -1)
    {
        error("mq_receive error");
    }
    clientID = atoi(message);
    if (clientID == -1)
    {
        error("server out of memory");
    }
    printf("Connected with server. Your ID is: %d\n", clientID);

    signal(SIGUSR1, handleConnect);
    while(true)
    {
        char command[128];
        printf("Waiting for commands...\n");
        fgets(command, 128, stdin);
        printf("%s", command);

        if (strncmp(command, "LIST", 4) == 0)
        {
            handleList();
        }
        else if (strncmp(command, "STOP", 4) == 0)
        {
            handleStop();
        }
        else if (strncmp(command, "CONNECT", 7) == 0)
        {
            int friend = atoi(command + 8);
            if (friend == clientID)
            {
                printf("Error: You can not chat with yourself.\n");
                continue;
            }

            sprintf(message, "%d:%d", clientID, friend);
            if (mq_send(serverQueueID, message, MAX_LENGTH, CONNECT) == -1)
            {
                error("mq_send error");
            }
            if (mq_receive(clientQueueID, message, MAX_LENGTH, NULL) == -1)
            {
                error("mq_receive error");
            }

            if (atoi(message) == -1)
            {
                printf("%s\n", message + 3);
            }
            else
            {
                raise(SIGUSR1);
            }
        }
        else
        {
            printf("Wrong command. Try LIST / STOP / CONNECT [NUM]\n");
        }
    }
}

// //////////////// Definitions ///////////////////////////////////////
void handleList()
{
    char message[MAX_LENGTH];
    // message.mtype = LIST;
    sprintf(message, "%02d", clientID);
    if (mq_send(serverQueueID, message, MAX_LENGTH, LIST) == -1)
    {
        error("mq_send error");
    }
    if (mq_receive(clientQueueID, message, MAX_LENGTH, NULL) == -1)
    {
        error("mq_receive error");
    }
    printf("%s", message + 2);
}

void handleStop()
{
    char message[MAX_LENGTH];
    sprintf(message, "%02d", clientID);
    if (mq_send(serverQueueID, message, MAX_LENGTH, STOP) == -1)
    {
        error("mq_send error");
    }
    printf("Client stops working.\n");
    mq_close(clientQueueID);
    mq_unlink(clientQueueName);
    mq_close(serverQueueID);
    exit(0);
}

void handleConnect()
{
    char message[MAX_LENGTH];
    if (mq_receive(clientQueueID, message, MAX_LENGTH, NULL) == -1)
    {
        error("mq_receive error");
    }
    printf("======================CHAT======================\n");
    printf("%s\nType DISCONNECT to disconnect.\n", message + 10);
    strtok(message, ":");
    strcpy(friendQueueName, strtok(NULL, ":"));
    friendQueueId = mq_open(friendQueueName, O_WRONLY);

    bool isFirst = true;
    bool isCaller = (atoi(message) != -1);
    char text[256];
    while(true)
    {
        if (!isFirst || isCaller)  // One client writes first.
        {
            printf("Send the message...\n");
            fgets(text, 256, stdin);
            if (strncmp(text, "DISCONNECT", 10) == 0)
            {
                if (mq_send(friendQueueId, message, MAX_LENGTH, DISCONNECT) == -1)
                {
                    error("mq_send error");
                }
                sprintf(message, "%d", clientID);
                if (mq_send(serverQueueID, message, MAX_LENGTH, DISCONNECT) == -1)
                {
                    error("mq_send error");
                }
                friendQueueId = -1;
                printf("Disconnected from the Chat\n");
                printf("======================CHAT======================\n");
                if (!isCaller)
                {
                    printf("Waiting for commands... (if nothing happens, type a few lines)\n");
                }
                return;
            }
            else
            {
                strcpy(message, text);
                if (mq_send(friendQueueId, message, MAX_LENGTH, CONNECT) == -1)
                {
                    error("mq_send error");
                }
            }
        }
        printf("Waiting for the message...\n");
        unsigned int priority;
        if (mq_receive(clientQueueID, message, MAX_LENGTH, &priority) == -1)
        {
            error("mq_receive error");
        }
        if (priority == DISCONNECT)
        {
            sprintf(message, "%d", clientID);
            if (mq_send(serverQueueID, message, MAX_LENGTH, DISCONNECT) == -1)
            {
                error("mq_send error");
            }
            friendQueueId = -1;
            printf("Disconnected from the Chat\n");
            printf("======================CHAT======================\n");
            if (!isCaller)
            {
                printf("Waiting for commands... (if nothing happes, type a few lines)\n");
            }
            return;
        }
        else
        {
            printf("Received: %s", message);
        }
        isFirst = false;
    }
}