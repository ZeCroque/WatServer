all: client.out server.out

client.out: client.c common.o
	gcc -Wall -g -o client.out client.c common.o

server.out: serveur.c common.o
	gcc -Wall -g -o serveur.out serveur.c common.o

common.o: common.c
	gcc -Wall -g -c -o common.o common.c

clean:
	rm -rf *.o
