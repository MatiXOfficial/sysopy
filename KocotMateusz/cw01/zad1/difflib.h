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
};

struct MainArray *createMainArray();

void diffTmp(char *filesString);

int saveTmpToArr(struct MainArray *mainarr);

int opNum(struct MainArray *mainarr, int i);

void removeOpBlock(struct MainArray *mainarr, int i);

void removeOp(struct MainArray *mainarr, int i, int j);

void freeAll(struct MainArray *mainarr);

#endif