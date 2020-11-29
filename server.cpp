#include "common.h"
#include <csignal>
#include <cerrno>  
#include <iostream>
#include <vector>

void sigHandler(int sigNo);
void handleCommunication(int serviceSocket);

enum class PeerType {None = 0, Client = 1, Host = 2};

struct PeerInfos
{
	struct sockaddr_in peerAdr;
	PeerType peerType;
};

int main()
{
	std::cout<<"Startup..."<<std::endl;
	
	signal(SIGINT, sigHandler);
	std::cout<<"Registered SIGINT handler"<<std::endl;

	int listenSocket=createTCPSocket(7777);
	fcntl(listenSocket, F_SETFL, O_NONBLOCK);
	if (listen(listenSocket, 5) == -1)
	{
		std::cerr<<"Error: Listen failed"<<std::endl;
		exit(1);
	}
	std::cout<<"Listen socket created"<<std::endl;

	std::vector<PeerInfos> peersInfos;
	peersInfos.reserve(50);
	while(true)
	{
		int serviceSocket;
		do
		{
			struct sockaddr_in clientAdr;
			int adrLength = sizeof(struct sockaddr_in);
			serviceSocket = accept(listenSocket, (struct sockaddr *)&clientAdr, (socklen_t *)&adrLength);
			if (serviceSocket == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
			{
				std::cerr<<"Error: Accept failed. Errno = "<<errno<<std::endl;
				exit(-1);
			}

			if(serviceSocket>-1)
			{
				std::cout<<"Connection accepted"<<std::endl;
				PeerInfos peerInfos = {clientAdr, PeerType::None};
				peersInfos.push_back(peerInfos);
			}
			
		} while (serviceSocket != -1);

		
			/*handleCommunication(serviceSocket);
			return 0;*/

	}
}

void sigHandler(int sigNo)
{
    printf("Received SIGINT, closing listenSocket\n");
	if(listenSocket>0)
	{
		close(listenSocket);
	}
}

void handleCommunication(int serviceSocket)
{
	/*char* inputBuffer;
	char *outputBuffer = "bien recu";
	int bufferLength;
	listenSocket = 0;
	
	inputBuffer =(char *)malloc(6 * sizeof(char));
	bufferLength = read(serviceSocket, inputBuffer, 6 * sizeof(char));
	printf("Received size:%d", bufferLength);
	printf("Message:%s",inputBuffer);

	// on envoie la réponse au client
	write(serviceSocket, outputBuffer, strlen(outputBuffer)+1);

	// on ferme les sockets
	close(serviceSocket);*/
}