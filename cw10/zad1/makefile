MAKEFLAGS += --no-print-directory
CC = gcc -Wall -g

all: build

build:
	$(CC) server.c -o server -lpthread
	$(CC) client.c -o client

clean:
	rm -f server
	rm -f client