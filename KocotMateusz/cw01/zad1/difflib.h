#ifndef DIFFLIB_H
#define DIFFLIB_H

struct MainArray
{
    int length;
    char **arr;
    int *used;
};

struct MainArray *createMainArray();
void diffTmp(char *filesString);

#endif