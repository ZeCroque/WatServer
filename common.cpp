#include "common.h"

#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

SOCKET createUDPSocket(int port)
{
  SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == INVALID_SOCKET)
  {
    std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
    return INVALID_SOCKET;
  }

  sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
  {
    std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
    closesocket(sock);
    return INVALID_SOCKET;
  }

  return sock;
}

