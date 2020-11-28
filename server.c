#include "common.h"
#include<signal.h>

static int listenSocket = 0;

void sigHandler(int sigNo);
int acceptConnection();
void handleCommunication(int serviceSocket);

int main()
{

	// socket d'écoute et de service
	int serviceSocket = 0;
	int pid;

	printf("Startup...\n");
	signal(SIGINT, sigHandler);
	printf("Registered SIGINT handler\n");
	listenSocket=createTCPSocket(7777);
	printf("Listen socket created\n");


	while(1)
	{
		serviceSocket = acceptConnection();
		pid=fork();
		if(!pid)
		{
			handleCommunication(serviceSocket);
			return 0;
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
}

int acceptConnection()
{
	static struct sockaddr_in clientAdr;
	if (listen(listenSocket, 5) == -1)
	{
		perror("Error: Listen failed");
		exit(1);
	}

	// on attend la connexion du client
	int adrLength = sizeof(struct sockaddr_in);
	int serviceSocket = accept(listenSocket, (struct sockaddr *)&clientAdr, (socklen_t *)&adrLength);
	if (serviceSocket == -1)
	{
		perror("Error: Accept failed");
		exit(1);
	}
	printf("Connection accepted");
	return serviceSocket;
}

void handleCommunication(int serviceSocket)
{
	char* inputBuffer;
	char *outputBuffer = "bien recu";
	int bufferLength;
	
	inputBuffer =(char *)malloc(6 * sizeof(char));
	bufferLength = read(serviceSocket, inputBuffer, 6 * sizeof(char));
	printf("Received size:%d", bufferLength);
	printf("Message:%s",inputBuffer);

	// on envoie la réponse au client
	write(serviceSocket, outputBuffer, strlen(outputBuffer)+1);

	// on ferme les sockets
	close(serviceSocket);
}
