#include "common.h"

#include <iostream>

int createUDPSocket(int port)
{
  int sock;
  static struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1)
  {
    perror("erreur création socket");
    return -1;
  }

  bzero(&addr, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  if( bind(sock, (struct sockaddr*)&addr, sizeof(addr))== -1 )
  {
    perror("erreur bind");
    return -1;
  }

  return sock;
}

