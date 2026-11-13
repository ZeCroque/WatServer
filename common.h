#ifndef common_h
#define common_h

#include <winsock2.h>
#include <ws2tcpip.h>

#define TAILLEBUF 64

SOCKET createUDPSocket(int port);

#endif
