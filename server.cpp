#include "common.h"
#include <csignal>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

enum class MessageType {Error = 0, HostPresentation = 1, ClientPresentation = 2, HostCount = 2, ClientConnectionRequest = 3, HostAddress = 4, HostDisconnected = 5};

struct Message
{
	MessageType messageType;
	struct sockaddr_in adr;
};

enum class PeerType {None = 0, Client = 1, Host = 2};

struct PeerInfos
{
	struct sockaddr_in peerAdr;
	PeerType peerType;
};

static SOCKET listenSocket = INVALID_SOCKET;
static std::vector<PeerInfos*> hostsInfos;
static std::vector<PeerInfos*> clientsInfos;

void sigHandler(int sigNo);
void handleCommunication(std::vector<PeerInfos*>& hostsInfos,  std::vector<PeerInfos*> clientsInfos);

int main()
{
	std::cout<<"Startup..."<<std::endl;
	
	signal(SIGINT, sigHandler);
	std::cout<<"Registered SIGINT handler."<<std::endl;

	WSADATA wsaData;
	int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0)
	{
		std::cerr << "WSAStartup failed: " << startupResult << std::endl;
		return 1;
	}

	listenSocket=createUDPSocket(7777);
	if (listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 1;
	}
	std::cout<<"Socket created."<<std::endl;
	
	hostsInfos.reserve(25);
	clientsInfos.reserve(25);
	while(true)
	{
		handleCommunication(hostsInfos, clientsInfos);
	}
}

void sigHandler(int sigNo)
{
    std::cout << "Received SIGINT, closing listenSocket, deleting addresses" << std::endl;
	if(listenSocket != INVALID_SOCKET)
	{
		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;
	}
	for(auto clientInfo : clientsInfos)
	{
		delete clientInfo;
	}
	for(auto hostInfos : hostsInfos)
	{
		delete hostInfos;
	}
	WSACleanup();
	exit(0);
}

void handleCommunication(std::vector<PeerInfos*>& hostsInfos,  std::vector<PeerInfos*> clientsInfos)
{
	struct sockaddr_in adr;
	std::memset(&adr, 0, sizeof(adr));
	int adrLength = sizeof(adr);
	
	Message message;
	std::memset(&message, 0, sizeof(Message));
	int receivedSize = recvfrom(listenSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<struct sockaddr*>(&adr), &adrLength);

	std::cout<< "Received message from: "<<inet_ntoa(adr.sin_addr)<<":"<<ntohs(adr.sin_port)<<std::endl;
	std::cout<<"Received size: "<<receivedSize<<std::endl;

	PeerInfos* peerInfos = new PeerInfos;
	peerInfos->peerAdr = adr;
	
	switch(message.messageType)
	{
	case MessageType::HostPresentation:
		peerInfos->peerType = PeerType::Host;
		hostsInfos.push_back(peerInfos);
		std::cout<< "Peer registered as host."<<std::endl;

		std::memset(&message, 0, sizeof(Message));
		message.adr = adr;
		message.messageType = MessageType::HostAddress;
		sendto(listenSocket, reinterpret_cast<char*>(&message), sizeof(Message),0,reinterpret_cast<struct sockaddr*>(&adr), sizeof(adr));
		std::cout<<"Sent host his own public address."<<std::endl;
		break;
	case MessageType::ClientPresentation:
	{
		peerInfos->peerType = PeerType::Client;
		clientsInfos.push_back(peerInfos);
		std::cout<<"Peer registered as client."<<std::endl;
			
		Message answer;
		std::memset(&answer, 0, sizeof(Message));
		answer.messageType=MessageType::HostCount;
		*reinterpret_cast<int*>(&answer.adr) = hostsInfos.size();
		sendto(listenSocket, reinterpret_cast<char*>(&answer), sizeof(Message), 0, reinterpret_cast<struct sockaddr*>(&adr), sizeof(adr));
		std::cout<<"Host count sent."<<std::endl;
			
		struct sockaddr_in* peersAdr = new struct sockaddr_in[hostsInfos.size()];
		for(unsigned int i = 0; i<hostsInfos.size(); ++i)
		{
			peersAdr[i] = hostsInfos[i]->peerAdr;
		}
		sendto(listenSocket, reinterpret_cast<char*>(peersAdr), sizeof(struct sockaddr_in)*hostsInfos.size(), 0, reinterpret_cast<struct sockaddr*>(&adr), sizeof(adr));
		delete[] peersAdr;
		std::cout<<"Addresses sent."<<std::endl;
		break;
	}
	case MessageType::ClientConnectionRequest:
	{
		std::cout<<"Connection request to host: "<<inet_ntoa(message.adr.sin_addr)<<":"<<ntohs(message.adr.sin_port)<<std::endl;
		Message answer = {MessageType::ClientConnectionRequest, adr};
		sendto(listenSocket, reinterpret_cast<char*>(&answer), sizeof(Message), 0, reinterpret_cast<struct sockaddr*>(&message.adr), sizeof(message.adr));
		std::cout<<"Connection request forwarded."<<std::endl;
		break;
	}
	case MessageType::HostDisconnected:
		std::cout<<"Host disconnected: "<<inet_ntoa(message.adr.sin_addr)<<":"<<ntohs(message.adr.sin_port)<<std::endl;
		for(auto it = hostsInfos.begin(); it != hostsInfos.end(); ++it)
		{
			if((*it)->peerAdr.sin_addr.s_addr == message.adr.sin_addr.s_addr && (*it)->peerAdr.sin_port == message.adr.sin_port)
			{
				delete *it;
				hostsInfos.erase(it);
				break;
			}
		}
		break;
	default: 
		std::cerr<<"Error: Unexpected message."<<std::endl;
		exit(-1);
	}
}
