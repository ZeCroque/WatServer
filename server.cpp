#include "common.h"
#include <csignal>
#include <cerrno>  
#include <iostream>
#include <vector>
#include <fcntl.h>

static int listenSocket = 0;

enum class MessageType {HostAdr = 0, ClientAdr = 1, HostCount = 2, ClientConnectionRequest = 3};

struct Message
{
	MessageType messageType;
	struct sockaddr_in adr;
};

enum class PeerType {None = 0, Client = 1, Host = 2};

struct PeerInfos
{
	int peerServiceSocket;
	struct sockaddr_in peerAdr;
	PeerType peerType;
};

void sigHandler(int sigNo);
void handleCommunication(PeerInfos& peerInfos);

int main()
{
	std::cout<<"Startup..."<<std::endl;
	
	signal(SIGINT, sigHandler);
	std::cout<<"Registered SIGINT handler"<<std::endl;

	listenSocket=createTCPSocket(7777);
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
				fcntl(serviceSocket, F_SETFL, O_NONBLOCK);
				PeerInfos peerInfos = {serviceSocket, clientAdr, PeerType::None};
				peersInfos.push_back(peerInfos);
			}
			
		} while (serviceSocket != -1);

		for(auto& peerInfos : peersInfos)
		{
			handleCommunication(peerInfos);
		}
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
	exit(0);
}

void handleCommunication(PeerInfos& peerInfos)
{
	std::cout<< inet_ntoa(peerInfos.peerAdr.sin_addr)<<":"<<ntohs(peerInfos.peerAdr.sin_port)<<std::endl;
				
				//peerInfos.peerType = PeerType::Client;
	
	Message message;
	int receivedSize = read(peerInfos.peerServiceSocket, &message, sizeof(message));
	if(receivedSize == -1 && errno != EAGAIN)
	{
		std::cerr<<"Error: Read error";
		exit(-1);
	}
	if(receivedSize>-1)
	{
		std::cout<< "Received message from:"<<inet_ntoa(peerInfos.peerAdr.sin_addr)<<":"<<ntohs(peerInfos.peerAdr.sin_port)<<std::endl;
		std::cout<<"Received size:"<<receivedSize<<std::endl;

		switch(message.messageType)
		{
		case MessageType::HostAdr:
			peerInfos.peerType = PeerType::Host;
			std::cout<< "Host waiting for party at:"<<inet_ntoa(message.adr.sin_addr)<<":"<<ntohs(message.adr.sin_port)<<std::endl;
			break;
		case MessageType::ClientAdr: 
			break;
		case MessageType::HostCount: 
			break;
		case MessageType::ClientConnectionRequest:
			break;
		default: 
			std::cerr<<"Error: Unknown message"<<std::endl;
			exit(-1);
		}
		close(peerInfos.peerServiceSocket);
	}
	
	
	

	//std::cout<< inet_ntoa(peerInfos.peerAdr.sin_addr)<<":"<<ntohs(peerInfos.peerAdr.sin_port)<<std::endl;
	
	// on envoie la réponse au client
	//write(serviceSocket, outputBuffer, strlen(outputBuffer)+1);

	// on ferme les sockets
	
}
