CC = gcc -Wall

all: build

build:
	$(CC) receiver.c -o receiver -lrt -lpthread
	$(CC) supervisor.c -o supervisor -lrt -lpthread
	$(CC) packer.c -o packer -lrt -lpthread
	$(CC) sender.c -o sender -lrt -lpthread

clean:
	rm -f receiver
	rm -f supervisor
	rm -f packer
	rm -f sender

example:
	make build
	./supervisor 4 3 2