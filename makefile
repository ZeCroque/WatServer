all: server.out

server.out: server.cpp common.o
	g++ -Wall -g -o server.out server.cpp common.o

common.o: common.cpp
	g++ -Wall -g -c -o common.o common.cpp

clean:
	rm -rf *.o

mrproper:
	rm -rf *.o; rm -rf *.out