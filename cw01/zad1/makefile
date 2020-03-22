CC = gcc -Wall

all: static shared clean

static:
	$(CC) -c difflib.c
	ar rcs libdifflib.a difflib.o

shared:
	$(CC) -fPIC -c difflib.c
	$(CC) -shared -fPIC -o libdifflib.so difflib.o 

clean:
	rm -f *.o

clean_all:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f diffTmpFile.txt