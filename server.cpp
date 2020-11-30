#include "common.h"
#include <csignal>
#include <cerrno>  
#include <iostream>
#include <vector>
#include <fcntl.h>

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

static int listenSocket = 0;
static std::vector<PeerInfos*> hostsInfos;
static std::vector<PeerInfos*> clientsInfos;

void sigHandler(int sigNo);
void handleCommunication(std::vector<PeerInfos*>& hostsInfos,  std::vector<PeerInfos*> clientsInfos);

int main()
{
	std::cout<<"Startup..."<<std::endl;
	
	signal(SIGINT, sigHandler);
	std::cout<<"Registered SIGINT handler."<<std::endl;

	listenSocket=createUDPSocket(7777);
	std::cout<<"Listen socket created."<<std::endl;
	
	hostsInfos.reserve(25);
	clientsInfos.reserve(25);
	while(true)
	{
		handleCommunication(hostsInfos, clientsInfos);
	}
}

void sigHandler(int sigNo)
{
    printf("Received SIGINT, closing listenSocket\n");
	if(listenSocket>0)
	{
		close(listenSocket);
	}
	for(auto clientInfo : clientsInfos)
	{
		delete clientInfo;
	}
	for(auto hostInfos : hostsInfos)
	{
		delete hostInfos;
	}
	exit(0);
}

void handleCommunication(std::vector<PeerInfos*>& hostsInfos,  std::vector<PeerInfos*> clientsInfos)
{
	struct sockaddr_in adr;
	memset(&adr, 0, sizeof(adr));
	int adrLength = sizeof(adr);
	
	Message message;
	message.messageType = MessageType::Error;
	int receivedSize = recvfrom(listenSocket, reinterpret_cast<void*>(&message), sizeof(Message), 0, reinterpret_cast<struct sockaddr*>(&adr), &adrLength);

	std::cout<< "Received message from: "<<inet_ntoa(adr.sin_addr)<<":"<<ntohs(adr.sin_port)<<std::endl;
	std::cout<<"Received size: "<<receivedSize<<std::endl;

	PeerInfos* peerInfos = new PeerInfos;
	peerInfos->peerAdr = adr;
	
	switch(message.messageType)
	{
	case MessageType::HostAdr:

		peerInfos->peerType = PeerType::Host;
		hostsInfos.push_back(peerInfos);
		std::cout<< "Peer registered as host."<<std::endl;
		break;
	case MessageType::ClientAdr:
	{
		peerInfos->peerType = PeerType::Client;
		clientsInfos.push_back(peerInfos);
		std::cout<<"Peer registered as client."<<std::endl<<"Sending client port..."<<std::endl;
			
		Message answer;
		memset(&answer, 0, sizeof(Message));
		answer.messageType = MessageType::HostCount;
		*reinterpret_cast<int*>(&answer.adr) = hostsInfos.size();
		sendto(listenSocket, reinterpret_cast<char*>(&answer), sizeof(Message), 0, reinterpret_cast<struct sockaddr*>(&adr), sizeof(adr));
		std::cout<<"Host count sent."<<std::endl<<"Sending hosts addresses..."<<std::endl;
			
		struct sockaddr_in* peersAdr = new struct sockaddr_in[hostsInfos.size()];
		for(unsigned int i = 0; i<hostsInfos.size(); ++i)
		{
			peersAdr[i] = hostsInfos[i]->peerAdr;
		}
		sendto(listenSocket, reinterpret_cast<char*>(peersAdr), sizeof(struct sockaddr_in)*hostsInfos.size(), 0, reinterpret_cast<struct sockaddr*>(&adr), sizeof(adr));
		std::cout<<"Addresses sent."<<std::endl;
		break;
	}
	case MessageType::ClientConnectionRequest:
	{
		std::cout<<"Connection request to host: "<<inet_ntoa(message.adr.sin_addr)<<":"<<ntohs(message.adr.sin_port)<<std::endl<<"Forwarding..."<<std::endl;
		Message answer;
		memset(&answer, 0, sizeof(Message));
		answer.messageType = MessageType::ClientConnectionRequest;
		answer.adr = adr;
		sendto(listenSocket, reinterpret_cast<char*>(&answer), sizeof(Message), 0, reinterpret_cast<struct sockaddr*>(&message.adr), sizeof(message.adr));
		std::cout<<"Connection request forwarded."<<std::endl;
		break;
	}
	default: 
		std::cerr<<"Error: Unexpected message."<<std::endl;
		exit(-1);
	}
}
