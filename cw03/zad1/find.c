#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <sys/wait.h>

void error(char *mes);

void printFileStat(const char *path, const struct stat *fileStat);
void findLib(char *path, int depth, char *relPath);

int mtime = -1, mtimemod = 0;   // 1: +, -1: -
int atime = -1, atimemod = 0;
int maxdepth = INT_MAX - 1;

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        error("Use ./main [path] arguments!");
    }
    char path[500];
    char rpath[500];
    strcpy(rpath, argv[1]);
    realpath(rpath, path);

    int i = 2;
    while (i < argc)
    {
        if (strcmp(argv[i], "-mtime") == 0)
        {
            i++;
            if (i >= argc)
            {
                error("Expected argument after -mtime!");
            }
            if (argv[i][0] == '+')
            {
                mtimemod = 1;
                mtime = atoi(argv[i] + 1);
            }
            else if (argv[i][0] == '-')
            {
                mtimemod = -1;
                mtime = atoi(argv[i] + 1);
            }
            else
            {
                mtime = atoi(argv[i]);   
            }
            i++;
        }
        else if (strcmp(argv[i], "-atime") == 0)
        {
            i++;
            if (i >= argc)
            {
                error("Expected argument after -atime!");
            }
            if (argv[i][0] == '+')
            {
                atimemod = 1;
                atime = atoi(argv[i] + 1);
            }
            else if (argv[i][0] == '-')
            {
                atimemod = -1;
                atime = atoi(argv[i] + 1);
            }
            else
            {
                atime = atoi(argv[i]);   
            }
            i++;
        }
        else if (strcmp(argv[i], "-maxdepth") == 0)
        {
            i++;
            if (i >= argc)
            {
                error("Expected argument after -maxdepth!");
            }
            else
            {
                maxdepth = atoi(argv[i]) - 1; 
            }
            i++;
        }
        else
        {
            error("Wrong argument!");
        }
    }
    findLib(path, 0, "");
}

void error(char *mes)
{
    perror(mes);
    exit(-1);
}

void printFileStat(const char *path, const struct stat *fileStat)
{
    char fileType[15];

    if (S_ISREG(fileStat->st_mode))
    {
        strcpy(fileType, "file");
    }
    else if (S_ISDIR(fileStat->st_mode))
    {
        strcpy(fileType, "dir");
    } 
    else if (S_ISCHR(fileStat->st_mode))
    {
        strcpy(fileType, "char dev");
    } 
    else if (S_ISBLK(fileStat->st_mode))
    {
        strcpy(fileType, "block dev");
    } 
    else if (S_ISFIFO(fileStat->st_mode))
    {
        strcpy(fileType, "fifo");
    } 
    else if (S_ISLNK(fileStat->st_mode))
    {
        strcpy(fileType, "slink");
    } 
    else if (S_ISSOCK(fileStat->st_mode))
    {
        strcpy(fileType, "socket");
    }

    time_t tm = fileStat->st_mtime;
    struct tm tminfo;
    localtime_r(&tm, &tminfo);
    char mtimestr[80];
    strftime(mtimestr, sizeof(mtimestr), "%c", &tminfo);

    time_t ta = fileStat->st_atime;
    struct tm tainfo;
    localtime_r(&ta, &tainfo);
    char atimestr[80];
    strftime(atimestr, sizeof(atimestr), "%c", &tainfo);

    time_t rawtime;
    time (&rawtime);

    int adiff = (rawtime - ta) / 86400;
    int mdiff = (rawtime - tm) / 86400;

    if ((mtime == -1 || (mtimemod == 0 && mtime == mdiff) || (mtimemod == -1 && mtime > mdiff) || (mtimemod == 1 && mtime < mdiff)) &&
        (atime == -1 || (atimemod == 0 && atime == adiff) || (atimemod == -1 && atime > adiff) || (atimemod == 1 && atime < adiff)))
            printf("path: %s, nlinks: %ld, type: %s, size: %ld, atime: %s, mtime: %s\n",
                    path, fileStat->st_nlink, fileType, fileStat->st_size, atimestr, mtimestr);
}

void findLib(char *path, int depth, char *relPath)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        return;
    }
    struct dirent *file = readdir(dir);
    file = readdir(dir);
    file = readdir(dir);
    struct stat *fileStat = calloc(1, sizeof(struct stat));
    if (depth == 0)
    {
        stat(path, fileStat);
        printFileStat(path, fileStat);
    }
    while (file != NULL)
    {
        char newpath[500];
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, file->d_name);
        stat(newpath, fileStat);
        //printFileStat(newpath, fileStat);
        if (S_ISDIR(fileStat->st_mode) && depth < maxdepth)
        {
            char newRelPath[500];
            strcpy(newRelPath, relPath);
            strcat(newRelPath, "/");
            strcat(newRelPath, file->d_name);
            pid_t childPid = fork();
            if (childPid == 0)
            {
                printf("%s, %d\n", newRelPath, getpid());
                execl("/bin/ls", "ls", "-l", NULL);
            }
            else
            {
                waitpid(childPid, NULL, 0);
                findLib(newpath, depth + 1, newRelPath);
            }
        }
        file = readdir(dir);
    }
    free(fileStat);
    closedir(dir);
}