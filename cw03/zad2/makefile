CC = gcc -Wall

all: build

build:
	$(CC) matrix.c -o matrix
	$(CC) generator.c -o generator
	$(CC) test.c -o test

clean:
	rm -f matrix
	rm -f generator
	rm -f test
	rm -f list
	rm -rf matrices

example:
	make build
	./generator 10 -10 10
	./matrix list 10 10 shared
	./test list
	./matrix list 10 10 separate
	./test list
	make clean