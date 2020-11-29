#include "common.h"
#include <csignal>
#include <cerrno>  
#include <iostream>
#include <vector>
#include <fcntl.h>

static int listenSocket = 0;

enum class MessageType {Error = -1, HostAdr = 0, ClientAdr = 1, HostCount = 2, ClientConnectionRequest = 3};

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
void handleCommunication(PeerInfos& peerInfos, std::vector<PeerInfos*>& hostsInfos,  std::vector<PeerInfos*> clientsInfos);

int main()
{
	std::cout<<"Startup..."<<std::endl;
	
	signal(SIGINT, sigHandler);
	std::cout<<"Registered SIGINT handler."<<std::endl;

	listenSocket=createTCPSocket(7777);
	fcntl(listenSocket, F_SETFL, O_NONBLOCK);
	if (listen(listenSocket, 5) == -1)
	{
		std::cerr<<"Error: Listen failed. Errno:"<<errno<<std::endl;
		exit(1);
	}
	std::cout<<"Listen socket created."<<std::endl;

	std::vector<PeerInfos> peersInfos;
	std::vector<PeerInfos*> hostsInfos;
	std::vector<PeerInfos*> clientsInfos;
	
	peersInfos.reserve(50);
	hostsInfos.reserve(25);
	clientsInfos.reserve(25);
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
			handleCommunication(peerInfos, hostsInfos, clientsInfos);
		}
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

void handleCommunication(PeerInfos& peerInfos, std::vector<PeerInfos*>& hostsInfos,  std::vector<PeerInfos*> clientsInfos)
{
	Message message;
	message.messageType = MessageType::Error;
	int receivedSize = read(peerInfos.peerServiceSocket, &message, sizeof(message));
	if(receivedSize == -1 && errno != EAGAIN)
	{
		std::cerr<<"Error: Read error. Errno:"<<errno;
		exit(-1);
	}
	if(receivedSize>0)
	{
		std::cout<< "Received message from: "<<inet_ntoa(peerInfos.peerAdr.sin_addr)<<":"<<ntohs(peerInfos.peerAdr.sin_port)<<std::endl;
		std::cout<<"Received size: "<<receivedSize<<std::endl;

		switch(message.messageType)
		{
		case MessageType::HostAdr:
			peerInfos.peerType = PeerType::Host;
			hostsInfos.push_back(&peerInfos);
			std::cout<< "Peer registered as host."<<std::endl;
			break;
		case MessageType::ClientAdr:
		{
			peerInfos.peerType = PeerType::Client;
			clientsInfos.push_back(&peerInfos);
			std::cout<<"Peer registered as host."<<std::endl<<"Sending host count..."<<std::endl;
				
			Message answer;
			memset(&answer, 0, sizeof(Message));
			answer.messageType = MessageType::HostCount;
			*reinterpret_cast<int*>(&answer.adr) = hostsInfos.size();
			write(peerInfos.peerServiceSocket, reinterpret_cast<char*>(&answer), sizeof(Message));
			std::cout<<"Host count sent."<<std::endl<<"Sending hosts addresses..."<<std::endl;
				
			struct sockaddr_in* peersAdr = new struct sockaddr_in[hostsInfos.size()];
			for(unsigned int i = 0; i<hostsInfos.size(); ++i)
			{
				peersAdr[i] = hostsInfos[i]->peerAdr;
			}
			write(peerInfos.peerServiceSocket, reinterpret_cast<char*>(peersAdr), sizeof(struct sockaddr_in)*hostsInfos.size());
			std::cout<<"Addresses sent."<<std::endl;
			break;
		}
		case MessageType::ClientConnectionRequest:
		{
			std::cout<<"Connection request to host: "<<inet_ntoa(message.adr.sin_addr)<<":"<<ntohs(message.adr.sin_port)<<std::endl<<"Forwarding..."<<std::endl;
			Message answer;
			memset(&answer, 0, sizeof(Message));
			answer.messageType = MessageType::ClientConnectionRequest;
			answer.adr = message.adr;
			for(auto* hostInfos : hostsInfos)
			{
				if(!strcmp(inet_ntoa(hostInfos->peerAdr.sin_addr), inet_ntoa(answer.adr.sin_addr)) && ntohs(hostInfos->peerAdr.sin_port) == ntohs(answer.adr.sin_port))
				{
					write(hostInfos->peerServiceSocket, reinterpret_cast<char*>(&answer), sizeof(Message));
					std::cout<<"Connection request forwarded."<<std::endl;
					break;
				}
			}
			break;
		}
		default: 
			std::cerr<<"Error: Unexpected message."<<std::endl;
			exit(-1);
		}
		//close(peerInfos.peerServiceSocket);
	}
	
	
	

	//std::cout<< inet_ntoa(peerInfos.peerAdr.sin_addr)<<":"<<ntohs(peerInfos.peerAdr.sin_port)<<std::endl;
	
	// on envoie la réponse au client
	//write(serviceSocket, outputBuffer, strlen(outputBuffer)+1);

	// on ferme les sockets
	
}
