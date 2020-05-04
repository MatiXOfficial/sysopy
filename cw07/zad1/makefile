CC = gcc -Wall

all: build

build:
	$(CC) receiver.c -o receiver
	$(CC) packer.c -o packer
	$(CC) sender.c -o sender
	$(CC) supervisor.c -o supervisor

clean:
	rm -f receiver
	rm -f packer
	rm -f supervisor
	rm -f sender

example:
	make build
	./supervisor 4 3 2