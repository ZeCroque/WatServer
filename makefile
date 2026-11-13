all: server.aarch64.out

server.aarch64.out: server.cpp common.o
	aarch64-linux-gnu-g++ -Wall -g -o server.aarch64.out server.cpp common.o

common.o: common.cpp
	aarch64-linux-gnu-g++ -Wall -g -c -o common.o common.cpp

clean:
	rm -rf *.o

mrproper:
	rm -rf *.o; rm -rf *.out