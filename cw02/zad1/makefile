MAKEFLAGS += --no-print-directory
CC = gcc -Wall

all: build

build:
	$(CC) main.c -o main

clean_all:
	rm -f main

test:
	make build

	@printf "SORT:\n" >> raport1.txt
	@printf "sys/lib recNum, recLen:\n" >> raport1.txt
	./main generate file 20000 1
	rm -f file2
	cp file file2
	@printf "sys 20000, 1 - " >> raport1.txt
	./main sort file 20000 1 sys >> raport1.txt
	@printf "lib 20000, 1 - " >> raport1.txt
	./main sort file2 20000 1 lib >> raport1.txt
	./main generate file 40000 1
	rm -f file2
	cp file file2
	@printf "sys 40000, 1 - " >> raport1.txt
	./main sort file 40000 1 sys >> raport1.txt
	@printf "lib 40000, 1 - " >> raport1.txt
	./main sort file2 40000 1 lib >> raport1.txt

	./main generate file 20000 4
	rm -f file2
	cp file file2
	@printf "\nsys 20000, 4 - " >> raport1.txt
	./main sort file 20000 4 sys >> raport1.txt
	@printf "lib 20000, 4 - " >> raport1.txt
	./main sort file2 20000 4 lib >> raport1.txt
	./main generate file 40000 4
	rm -f file2
	cp file file2
	@printf "sys 40000, 4 - " >> raport1.txt
	./main sort file 40000 4 sys >> raport1.txt
	@printf "lib 40000, 4 - " >> raport1.txt
	./main sort file2 40000 4 lib >> raport1.txt

	./main generate file 20000 512
	rm -f file2
	cp file file2
	@printf "\nsys 20000, 512 - " >> raport1.txt
	./main sort file 20000 512 sys >> raport1.txt
	@printf "lib 20000, 512 - " >> raport1.txt
	./main sort file2 20000 512 lib >> raport1.txt
	./main generate file 40000 512
	rm -f file2
	cp file file2
	@printf "sys 40000, 512 - " >> raport1.txt
	./main sort file 40000 512 sys >> raport1.txt
	@printf "lib 40000, 512 - " >> raport1.txt
	./main sort file2 40000 512 lib >> raport1.txt

	./main generate file 20000 1024
	rm -f file2
	cp file file2
	@printf "\nsys 20000, 1024 - " >> raport1.txt
	./main sort file 20000 1024 sys >> raport1.txt
	@printf "lib 20000, 1024 - " >> raport1.txt
	./main sort file2 20000 1024 lib >> raport1.txt
	./main generate file 40000 1024
	rm -f file2
	cp file file2
	@printf "sys 40000, 1024 - " >> raport1.txt
	./main sort file 40000 1024 sys >> raport1.txt
	@printf "lib 40000, 1024 - " >> raport1.txt
	./main sort file2 40000 1024 lib >> raport1.txt

	./main generate file 2000 4096
	rm -f file2
	cp file file2
	@printf "\nsys 2000, 4096 - " >> raport1.txt
	./main sort file 2000 4096 sys >> raport1.txt
	@printf "lib 2000, 4096 - " >> raport1.txt
	./main sort file2 2000 4096 lib >> raport1.txt
	./main generate file 4000 4096
	rm -f file2
	cp file file2
	@printf "sys 4000, 4096 - " >> raport1.txt
	./main sort file 4000 4096 sys >> raport1.txt
	@printf "lib 4000, 4096 - " >> raport1.txt
	./main sort file2 4000 4096 lib >> raport1.txt

	./main generate file 2000 8192
	rm -f file2
	cp file file2
	@printf "\nsys 2000, 8192 - " >> raport1.txt
	./main sort file 2000 8192 sys >> raport1.txt
	@printf "lib 2000, 8192 - " >> raport1.txt
	./main sort file2 2000 8192 lib >> raport1.txt
	./main generate file 4000 8192
	rm -f file2
	cp file file2
	@printf "sys 4000, 8192 - " >> raport1.txt
	./main sort file 4000 8192 sys >> raport1.txt
	@printf "lib 4000, 8192 - " >> raport1.txt
	./main sort file2 4000 8192 lib >> raport1.txt


	

	@printf "COPY:\n" >> raport1.txt
	@printf "sys/lib recNum, recLen:\n" >> raport1.txt
	./main generate file 20000 1
	@printf "sys 20000, 1 - " >> raport1.txt
	./main copy file file2 20000 1 sys >> raport1.txt
	@printf "lib 20000, 1 - " >> raport1.txt
	./main copy file file2 20000 1 lib >> raport1.txt
	./main generate file 40000 1
	@printf "sys 40000, 1 - " >> raport1.txt
	./main copy file file2 40000 1 sys >> raport1.txt
	@printf "lib 40000, 1 - " >> raport1.txt
	./main copy file file2 40000 1 lib >> raport1.txt

	./main generate file 20000 4
	@printf "\nsys 20000, 4 - " >> raport1.txt
	./main copy file file2 20000 4 sys >> raport1.txt
	@printf "lib 20000, 4 - " >> raport1.txt
	./main copy file file2 20000 4 lib >> raport1.txt
	./main generate file 40000 4
	@printf "sys 40000, 4 - " >> raport1.txt
	./main copy file file2 40000 4 sys >> raport1.txt
	@printf "lib 40000, 4 - " >> raport1.txt
	./main copy file file2 40000 4 lib >> raport1.txt

	./main generate file 20000 512
	@printf "\nsys 20000, 512 - " >> raport1.txt
	./main copy file file2 20000 512 sys >> raport1.txt
	@printf "lib 20000, 512 - " >> raport1.txt
	./main copy file file2 20000 512 lib >> raport1.txt
	./main generate file 40000 512
	@printf "sys 40000, 512 - " >> raport1.txt
	./main copy file file2 40000 512 sys >> raport1.txt
	@printf "lib 40000, 512 - " >> raport1.txt
	./main copy file file2 40000 512 lib >> raport1.txt

	./main generate file 20000 1024
	@printf "\nsys 20000, 1024 - " >> raport1.txt
	./main copy file file2 20000 1024 sys >> raport1.txt
	@printf "lib 20000, 1024 - " >> raport1.txt
	./main copy file file2 20000 1024 lib >> raport1.txt
	./main generate file 40000 1024
	@printf "sys 40000, 1024 - " >> raport1.txt
	./main copy file file2 40000 1024 sys >> raport1.txt
	@printf "lib 40000, 1024 - " >> raport1.txt
	./main copy file file2 40000 1024 lib >> raport1.txt

	./main generate file 2000 4096
	@printf "\nsys 2000, 4096 - " >> raport1.txt
	./main copy file file2 2000 4096 sys >> raport1.txt
	@printf "lib 2000, 4096 - " >> raport1.txt
	./main copy file file2 2000 4096 lib >> raport1.txt
	./main generate file 4000 4096
	@printf "sys 4000, 4096 - " >> raport1.txt
	./main copy file file2 4000 4096 sys >> raport1.txt
	@printf "lib 4000, 4096 - " >> raport1.txt
	./main copy file file2 4000 4096 lib >> raport1.txt

	./main generate file 2000 8192
	@printf "\nsys 2000, 8192 - " >> raport1.txt
	./main copy file file2 2000 8192 sys >> raport1.txt
	@printf "lib 2000, 8192 - " >> raport1.txt
	./main copy file file2 2000 8192 lib >> raport1.txt
	./main generate file 4000 8192
	@printf "sys 4000, 8192 - " >> raport1.txt
	./main copy file file2 4000 8192 sys >> raport1.txt
	@printf "lib 4000, 8192 - " >> raport1.txt
	./main copy file file2 4000 8192 lib >> raport1.txt


	rm -f file
	rm -f file2