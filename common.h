#ifndef common_h
#define common_h

//#include <stdio.h>
#include <cstdlib>
//#include <string.h>
#include <unistd.h>
//#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <errno.h>

#define TAILLEBUF 64

int createTCPSocket(int port);

#endif
