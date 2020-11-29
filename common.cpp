#include "common.h"


int createTCPSocket(int port)
{
  int sock;
  static struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    perror("creation socket");
    exit(1);
  }

  bzero((char *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  if( bind(sock, (struct sockaddr*)&addr, sizeof(addr))== -1)
  {
    perror("erreur bind socket");
    exit(1);
  }
  return sock;
}
