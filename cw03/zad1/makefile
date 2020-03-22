CC = gcc -Wall

all: build

build:
	$(CC) find.c -o find

clean:
	rm find

test:
	make build
	./find directory
	make clean