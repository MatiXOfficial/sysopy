CC = gcc -Wall
MAKEFLAGS += --no-print-directory

all: build

build:
	$(CC) main.c -o main
	$(CC) exec_fun.c -o exec_fun

clean:
	rm -f main
	rm -f exec_fun

task:
	make build

	@./main ignore
	@./main handler
	@./main mask
	@./main pending

	make clean