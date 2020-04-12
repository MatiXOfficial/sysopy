CC = gcc -Wall

all: build

build:
	$(CC) main.c -o main

clean:
	rm -f main

example:
	make build
	./main file.txt
	make clean