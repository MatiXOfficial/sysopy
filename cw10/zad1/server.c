#include "common.h"

#define PING_TIME 4

void exitFun();
int isClientInQueue(char *clientName);
int findClientIdx(int fd);
int acceptClient(int fd_client);
char checkGridResult(int id, char symbol); // 0 - continue, 1 - symbol win, 2 - draw
void handleDisconnection(int id1, int id2);
void disconnect(int id);

void *ping();

struct ClientsQueue
{
    int fds[MAX_CLIENTS];
    int partners[MAX_CLIENTS];
    char names[MAX_CLIENTS][MAX_NAME_LENGTH];
    char symbols[MAX_CLIENTS]; // 'X' / 'O'
    char *grids[MAX_CLIENTS];
    int first;
    int size;
    int isActive[MAX_CLIENTS];
};

char unixPath[108];

int fd_inetServer;
int fd_unixServer;
int fd_epoll;

struct epoll_event epollEventArr[MAX_CLIENTS];
struct ClientsQueue cq = {.first = 0, .size = 0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    if (argc != 3)
    {
        error("Wrong number of arguments! Try ./server [TCP port] [unix socket path]");
    }
    strcpy(unixPath, argv[2]);

    ////// inet /////
    fd_inetServer = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_inetServer == -1)
        error("inet; socket error");

    struct sockaddr_in addr_inetServer = {.sin_family = AF_INET, .sin_port = htobe16(atoi(argv[1])), 
                                          .sin_addr = {.s_addr = INADDR_ANY}};

    if (bind(fd_inetServer, (struct sockaddr *)&addr_inetServer, sizeof(addr_inetServer)) == -1)
        error("inet; bind error");

    ///// unix /////
    fd_unixServer = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_unixServer == -1)
        error("unix: socket error");

    struct sockaddr_un addr_unixServer = {.sun_family = AF_UNIX};
    strcpy(addr_unixServer.sun_path, unixPath);

    if (bind(fd_unixServer, (struct sockaddr *)&addr_unixServer, sizeof(addr_unixServer)) == -1)
        error("unix: socket error");

    ///// epoll //////
    fd_epoll = epoll_create1(0);
    struct epoll_event epollEvent = {.events = EPOLLIN};

    epollEvent.data.fd = fd_inetServer;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_inetServer, &epollEvent) == -1)
        error("inet: epoll_ctl error");

    epollEvent.data.fd = fd_unixServer;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_unixServer, &epollEvent) == -1)
        error("unix: epoll_ctl error");

    //////////
    signal(SIGINT, exitFun);
    listen(fd_inetServer, MAX_CLIENTS);
    listen(fd_unixServer, MAX_CLIENTS);
    pthread_t thread;
    if (pthread_create(&thread, NULL, ping, NULL) == -1)
    {
        error("pthread_create error");
    }
    while(1)
    {
        int eventsNum = epoll_wait(fd_epoll, epollEventArr, MAX_CLIENTS, -1);
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < eventsNum; i++)
        {
            int fd = epollEventArr[i].data.fd;

            // If a new player
            if (fd == fd_inetServer || fd == fd_unixServer)
            {
                int id2 = acceptClient(fd);
                if (id2 != -1)
                {
                    if (cq.partners[id2] != -1)
                    {
                        int fd_client1 = cq.fds[cq.partners[id2]];
                        int id1 = (MAX_CLIENTS + id2 - 1) % MAX_CLIENTS;
                        printf("Client %s (%d) and %s (%d) are now in game.\n", 
                                cq.names[id1], id1, cq.names[id2], id2);
                        int fd_client2 = cq.fds[cq.partners[id1]];

                        int length = strlen(cq.names[id2]) + 1;
                        write(fd_client1, &length, sizeof(int)); // send opponents' names
                        write(fd_client1, cq.names[id2], length);

                        length = strlen(cq.names[id1]) + 1;
                        write(fd_client2, &length, sizeof(int)); 
                        write(fd_client2, cq.names[id1], length);

                        cq.grids[id1] = calloc(9, sizeof(char));
                        cq.grids[id2] = cq.grids[id1];
                        for (int i = 0; i < 9; i++)
                        {
                            cq.grids[id1][i] = ' ';
                        }

                        write(fd_client1, "012345678", 9); // send grid numbers
                        write(fd_client2, "012345678", 9);

                        int which = rand() % 2; // randomly choosing starting player
                        if (which == 0)         // player with 'X' will start
                        {
                            cq.symbols[id2] = 'X';
                            cq.symbols[id1] = 'O';
                        }
                        else
                        {
                            cq.symbols[id2] = 'O';
                            cq.symbols[id1] = 'X';
                        }
                        write(fd_client1, &cq.symbols[id1], 1); // send symbol
                        write(fd_client2, &cq.symbols[id2], 1);
                    }
                }
            }
            // player already registered
            else
            {
                int id1 = findClientIdx(fd);
                int id2 = cq.partners[id1];
                int fd_client2 = cq.fds[id2];

                char sign[2];
                if (read(fd, sign, 1) == 0)
                {
                    if (id2 == -1)
                    {
                        disconnect(id1);
                    }
                    else
                    {
                        write(fd_client2, "D", 1);
                        handleDisconnection(id1, id2);
                    }
                    continue;
                }
                sign[1] = '\0';
                int choice = atoi(sign);
                if (cq.grids[id1][choice] != ' ')
                {
                    write(fd, "3", 1);
                    continue;
                }
                char symbol = cq.symbols[id1];
                cq.grids[id1][choice] = symbol;

                char result = checkGridResult(id1, symbol);
                write(fd, &result, 1);
                write(fd, cq.grids[id1], 9);

                write(fd_client2, &result, 1);
                write(fd_client2, cq.grids[id1], 9);

                if (result != '0')
                {
                    handleDisconnection(id1, id2);
                }
                cq.isActive[id1] = 1;
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    raise(SIGINT);
}


///////////// Definitons ////////////////
void exitFun()
{
    if (close(fd_inetServer) == -1)
        error("inet: close error");
    if (close(fd_unixServer) == -1)
        error("unix: close error");

    pthread_mutex_destroy(&mutex);
    unlink(unixPath);
    exit(0);
}

int isClientInQueue(char *clientName)
{
    if (cq.size == 0)
        return 0;
    if (strcmp(cq.names[cq.first], clientName) == 0)
        return 1;
    for (int i = (cq.first + 1) % MAX_CLIENTS; i != (cq.first + cq.size) % MAX_CLIENTS; i = (i + 1) % MAX_CLIENTS)
    {
        if (strcmp(cq.names[i], clientName) == 0)
            return 1;
    }
    return 0;
}

int findClientIdx(int fd)
{
    if (cq.fds[cq.first] == fd)
        return cq.first;
    for (int i = (cq.first + 1) % MAX_CLIENTS; i != (cq.first + cq.size) % MAX_CLIENTS; i = (i + 1) % MAX_CLIENTS)
    {
        if (cq.fds[i] == fd)
            return i;
    }
    return -1;
}

int acceptClient(int fd)
{
    int fd_client = accept(fd, NULL, NULL);
    if (fd_client == -1)
        error("client accept error");

    int last = (cq.first + cq.size) % MAX_CLIENTS;
    char clientName[MAX_NAME_LENGTH];

    int length;
    read(fd_client, &length, sizeof(int));
    read(fd_client, clientName, length);

    if (cq.size >= MAX_CLIENTS)
    {
        write(fd_client, "1", 1);
        return -1;
    }
    if (isClientInQueue(clientName))
    {
        write(fd_client, "2", 1);
        return -1;
    }
    write(fd_client, "0", 1);

    strcpy(cq.names[last], clientName);
    int prev = (MAX_CLIENTS + last - 1) % MAX_CLIENTS;
    cq.size++;
    cq.fds[last] = fd_client;
    cq.isActive[last] = 1;

    if (cq.size > 1 && cq.partners[prev] == -1)
    {
        cq.partners[prev] = last;
        cq.partners[last] = prev;
    }
    else
    {
        cq.partners[last] = -1;
    }

    printf("Client %s (%d) connected sucessfully.\n", cq.names[last], last);

    struct epoll_event epollEvent = {.events = EPOLLIN};
    epollEvent.data.fd = fd_client;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_client, &epollEvent) == -1)
        error("client: epoll_ctl error");

    return last;
}

char checkGridResult(int id, char symbol)
{
    char *grid = cq.grids[id];
    if (grid[0] == symbol && grid[1] == symbol && grid[2] == symbol)
        return '1';
    if (grid[3] == symbol && grid[4] == symbol && grid[5] == symbol)
        return '1';
    if (grid[6] == symbol && grid[7] == symbol && grid[8] == symbol)
        return '1';

    if (grid[0] == symbol && grid[3] == symbol && grid[6] == symbol)
        return '1';
    if (grid[1] == symbol && grid[4] == symbol && grid[7] == symbol)
        return '1';
    if (grid[2] == symbol && grid[5] == symbol && grid[8] == symbol)
        return '1';

    if (grid[0] == symbol && grid[4] == symbol && grid[8] == symbol)
        return '1';
    if (grid[2] == symbol && grid[4] == symbol && grid[6] == symbol)
        return '1';

    for (int i = 0; i < 9; i++)
    {
        if (grid[i] == ' ')
            return '0';
    }
    return '2';
}

void handleDisconnection(int id1, int id2)
{
    int fd1 = cq.fds[id1];
    int fd2 = cq.fds[id2];

    struct epoll_event epollEvent = {.events = EPOLLIN};
    epollEvent.data.fd = fd1;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd1, &epollEvent) == -1)
        error("epoll_ctl (del) error");
    epollEvent.data.fd = fd2;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd2, &epollEvent) == -1)
        error("epoll_ctl (del) error");

    free(cq.grids[id1]);
    int i = (MAX_CLIENTS + (id1 < id2 ? id1 : id2) - 1) % MAX_CLIENTS;
    for (; i != (MAX_CLIENTS + cq.first - 1) % MAX_CLIENTS; i = (MAX_CLIENTS + i - 1) % MAX_CLIENTS)
    {
        int j = (i + 2) % MAX_CLIENTS;
        cq.fds[j] = cq.fds[i];
        cq.grids[j] = cq.grids[i];
        strcpy(cq.names[j], cq.names[i]);
        cq.partners[j] = cq.partners[i];
        cq.symbols[j] = cq.symbols[i];
        cq.isActive[j] = cq.isActive[i];
    }
    if (cq.size > 2)
        cq.first += 2;
    cq.size -= 2;
}

void disconnect(int id)
{
    int fd = cq.fds[id];

    struct epoll_event epollEvent = {.events = EPOLLIN};
    epollEvent.data.fd = fd;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd, &epollEvent) == -1)
        error("epoll_ctl (del) error");

    int i = (MAX_CLIENTS + id - 1) % MAX_CLIENTS;
    for (; i != (MAX_CLIENTS + cq.first - 1) % MAX_CLIENTS; i = (MAX_CLIENTS + i - 1) % MAX_CLIENTS)
    {
        int j = (i + 1) % MAX_CLIENTS;
        cq.fds[j] = cq.fds[i];
        cq.grids[j] = cq.grids[i];
        strcpy(cq.names[j], cq.names[i]);
        cq.partners[j] = cq.partners[i];
        cq.symbols[j] = cq.symbols[i];
        cq.isActive[j] = cq.isActive[i];
    }
    if (cq.size > 1)
        cq.first += 1;
    cq.size -= 1;
}

void *ping()
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < cq.size; i++)
        {
            int id1 = (cq.first + i) % MAX_CLIENTS;
            int id2 = cq.partners[id1];
            if (cq.isActive[id1] == 0)
            {
                if (id2 == -1)
                {
                    write(cq.fds[id1], "T", 1);
                    disconnect(id1);
                }
                else if (cq.isActive[id2] == 0)
                {
                    write(cq.fds[id1], "T", 1);
                    write(cq.fds[id2], "D", 1);
                    handleDisconnection(id1, id2);
                }
            }
        }
        for (int i = 0; i < cq.size; i++)
        {
            int id = (cq.first + i) % MAX_CLIENTS;
            cq.isActive[id] = 0;
        }
        pthread_mutex_unlock(&mutex);
        sleep(PING_TIME);
    }
    return NULL;
}