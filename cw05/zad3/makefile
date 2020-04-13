CC = gcc -Wall

all: build

build:
	$(CC) producer.c -o producer
	$(CC) consumer.c -o consumer
	$(CC) superior.c -o superior
	$(CC) generator.c -o generator

producer:
	$(CC) producer.c -o producer

consumer:
	$(CC) consumer.c -o consumer

superior:
	$(CC) superior.c -o superior

generator:
	$(CC) generator.c -o generator

clean_pipe:
	rm -f /tmp/pipe

clean:
	rm -f producer
	rm -f consumer
	rm -f superior
	rm -f generator

	rm -f /tmp/pipe

	rm -f results.txt
	rm -f prod_text1.txt
	rm -f prod_text2.txt
	rm -f prod_text3.txt
	rm -f prod_text4.txt
	rm -f prod_text5.txt
	rm -f prod_text6.txt
	rm -f prod_text7.txt
	rm -f prod_text8.txt
	rm -f prod_text9.txt
	rm -f prod_text10.txt

example:
	make build
	./superior
	make clean