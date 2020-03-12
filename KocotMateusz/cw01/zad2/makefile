CC = gcc -Wall

all: static clean

static:
	$(CC) -c difflib.c
	ar rcs libdifflib.a difflib.o

clean:
	rm -f *.o