#ifndef common_h
#define common_h

//#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
//#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <errno.h>

#define TAILLEBUF 64

int createUDPSocket(int port);

#endif
