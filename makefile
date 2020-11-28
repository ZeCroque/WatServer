all: client.out server.out

client.out: client.c common.o
	gcc -Wall -g -o client.out client.c common.o

server.out: server.c common.o
	gcc -Wall -g -o server.out server.c common.o

common.o: common.c
	gcc -Wall -g -c -o common.o common.c

clean:
	rm -rf *.o

mrproper:
	rm -rf *.o; rm -rf *.out