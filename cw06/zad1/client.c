#include "common_header.h"

void handleList();
void handleStop();
void handleConnect();

int serverQueueID;
int clientQueueID;
int clientID;
int friendQueueId = -1;

int main()
{
    // Server queue
    key_t key = ftok(getenv("HOME"), SERVER_PROJ_ID);
    if (key == -1)
    {
        error("ftok error (server queue)");
    }
    serverQueueID = msgget(key, 0777);
    if (serverQueueID == -1)
    {
        error("msgget error (server queue");
    }


    // Client queue
    key = ftok(getenv("HOME"), currentClientProjID++);
    if (key == -1)
    {
        error("ftok error (client queue)");
    }
    clientQueueID = msgget(key, IPC_CREAT | 0777);
    if (clientQueueID == -1)
    {
        error("msgget error (client queue)");
    }

    signal(SIGINT, handleStop);

    struct Message message;
    message.mtype = INIT;
    message.info1 = clientQueueID;
    message.info2 = getpid();
    if (msgsnd(serverQueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }

    if (msgrcv(clientQueueID, &message, messageSize, INIT, 0) == -1)
    {
        error("msgrcv error");
    }
    clientID = message.info1;
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
            printf("lista\n");
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
            message.mtype = CONNECT;
            message.info1 = clientID;
            message.info2 = friend;
            if (msgsnd(serverQueueID, &message, messageSize, 0) == -1)
            {
                error("msgsnd error");
            }
            if (msgrcv(clientQueueID, &message, messageSize, CONNECT, 0) == -1)
            {
                error("msgrcv error");
            }

            if (message.info1 == -1)
            {
                printf("%s\n", message.text);
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

//////////////// Definitions ///////////////////////////////////////
void handleList()
{
    struct Message message;
    message.mtype = LIST;
    message.info1 = clientID;
    if (msgsnd(serverQueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }
    if (msgrcv(clientQueueID,& message, messageSize, LIST, 0) == -1)
    {
        error("msgrcv error");
    }
    printf("%s", message.text);
}

void handleStop()
{
    struct Message message;
    message.mtype = STOP;
    message.info1 = clientID;
    if (msgsnd(serverQueueID, &message, messageSize, 0) == -1)
    {
        error("msgsnd error");
    }
    printf("Client stops working.\n");
    msgctl(clientQueueID, IPC_RMID, 0);
    exit(0);
}

void handleConnect()
{
    struct Message message;
    if (msgrcv(clientQueueID, &message, messageSize, CONNECT, 0) == -1)
    {
        error("msgrcv error");
    }
    printf("======================CHAT======================\n");
    printf("%s\nType DISCONNECT to disconnect.\n", message.text);
    friendQueueId = message.info2;

    bool isFirst = true;
    bool isCaller = (message.info1 != -1);
    char text[256];
    while(true)
    {
        if (!isFirst || isCaller)  // One client writes first.
        {
            printf("Send the message...\n");
            fgets(text, 256, stdin);
            if (strncmp(text, "DISCONNECT", 10) == 0)
            {
                message.mtype = DISCONNECT;
                if (msgsnd(friendQueueId, &message, messageSize, 0) == -1)
                {
                    error("msgsnd error");
                }
                message.info1 = clientID;
                if (msgsnd(serverQueueID, &message, messageSize, 0) == -1)
                {
                    error("msgsnd error");
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
                strcpy(message.text, text);
                if (msgsnd(friendQueueId, &message, messageSize, 0) == -1)
                {
                    error("msgsnd error");
                }
            }
        }
        printf("Waiting for the message...\n");
        if (msgrcv(clientQueueID, &message, messageSize, 0, 0) == -1)
        {
            error("msgrcv error");
        }
        if (message.mtype == DISCONNECT)
        {
            message.mtype = DISCONNECT;
            message.info1 = clientID;
            if (msgsnd(serverQueueID, &message, messageSize, 0) == -1)
            {
                error("msgsnd error");
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
            printf("Received: %s", message.text);
        }
        isFirst = false;
    }
}