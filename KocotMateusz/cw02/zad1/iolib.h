#ifndef IOLIB_H
#define IOLIB_H

void generate(char *fileName, int recNum, int recLen);

void sysSort(char *fileName, int recNum, int recLen);
void libSort(char *fileName, int recNum, int recLen);

void sysCopy(char *fileName1, char *fileName2, int recNum, int recLen);
void libCopy(char *fileName1, char *fileName2, int recNum, int recLen);

#endif