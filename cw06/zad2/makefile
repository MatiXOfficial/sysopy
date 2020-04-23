CC = gcc -g -Wall

all: build

build:
	$(CC) server.c -o server -lrt
	$(CC) client.c -o client -lrt

clean:
	rm -f server
	rm -f client