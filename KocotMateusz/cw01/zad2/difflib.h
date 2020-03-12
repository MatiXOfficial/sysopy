#ifndef DIFFLIB_H
#define DIFFLIB_H

struct OperationsArray
{
    char **op;
    int length;
};

struct MainArray
{
    struct OperationsArray **oparr;
    int length;
    int n;
};

struct MainArray *createMainArray(int n);

void saveDiffToTmp(char *filesString);

int saveTmpToArr(struct MainArray *mainarr);

int opNum(struct MainArray *mainarr, int i);

void removeOpBlock(struct MainArray *mainarr, int i);

void removeOp(struct MainArray *mainarr, int i, int j);

void freeAll(struct MainArray *mainarr);

#endif