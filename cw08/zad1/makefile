MAKEFLAGS += --no-print-directory
CC = gcc -Wall -g

all: build

build:
	$(CC) main.c -o main -lpthread

clean:
	rm -f main

times:
	make build

	@printf "==>sign - 1 thread:\n" > times.txt
	./main 1 sign body.pgm hist.txt >> times.txt
	@printf "==>sign - 2 threads:\n" >> times.txt
	./main 2 sign body.pgm hist.txt >> times.txt
	@printf "==>sign - 4 threads:\n" >> times.txt
	./main 4 sign body.pgm hist.txt >> times.txt
	@printf "==>sign - 8 threads:\n" >> times.txt
	./main 8 sign body.pgm hist.txt >> times.txt

	@printf "===========================\n" >> times.txt
	@printf "==>block - 1 thread:\n" >> times.txt
	./main 1 block body.pgm hist.txt >> times.txt
	@printf "==>block - 2 threads:\n" >> times.txt
	./main 2 block body.pgm hist.txt >> times.txt
	@printf "==>block - 4 threads:\n" >> times.txt
	./main 4 block body.pgm hist.txt >> times.txt
	@printf "==>block - 8 threads:\n" >> times.txt
	./main 8 block body.pgm hist.txt >> times.txt

	@printf "===========================\n" >> times.txt
	@printf "==>interleaved - 1 thread:\n" >> times.txt
	./main 1 interleaved body.pgm hist.txt >> times.txt
	@printf "==>interleaved - 2 threads:\n" >> times.txt
	./main 2 interleaved body.pgm hist.txt >> times.txt
	@printf "==>interleaved - 4 threads:\n" >> times.txt
	./main 4 interleaved body.pgm hist.txt >> times.txt
	@printf "==>interleaved - 8 threads:\n" >> times.txt
	./main 8 interleaved body.pgm hist.txt >> times.txt

	make clean