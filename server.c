#include "common.h"

int main(int argc, char** argv)
{
  // adresse socket coté client
  static struct sockaddr_in addr_client;
  // longueur adresse
  int lg_addr;
  // socket d'écoute et de service
  int listenSocket, serviceSocket;
  // chaîne reçue du client
  char *chaine_recue;
  // chaîne renvoyée au client
  char *reponse = "bien recu";
  // nombre d'octets reçus ou envoyés
  int nb_octets;

  long lSize;

  // création socket TCP d'écoute
  if(!listenSocket=createTCPSocket(7777))
{  	
	perror("socket creation error");
  	exit(-1);
}
  	printf("connexion acceptée!");

  // configuration socket écoute : 5 connexions max en attente
  if (listen(listenSocket, 1) == -1)
  {
    perror("erreur listen");
    exit(1);
  }

  // on attend la connexion du client
  lg_addr = sizeof(struct sockaddr_in);
  serviceSocket = accept(listenSocket, (struct sockaddr *)&addr_client, (socklen_t *)&lg_addr);
  if (serviceSocket == -1)
  {
    perror("erreur accept");
    exit(1);
  }
  printf("connexion acceptée!");
  // la connexion est établie, on attend les données envoyées par le client
  /*nb_octets = read(serviceSocket, &lSize, sizeof(long));
  printf("taille reçue :%ld\n",lSize);
  chaine_recue =(char *)malloc(lSize * sizeof(char));
  nb_octets = read(serviceSocket, chaine_recue, lSize);
  printf("taille effective:%d", nb_octets);
  
  FILE* fichier = NULL;

  fichier = fopen("reponse.jpg", "wb");
  int i=0;
  for(i=0; i<lSize; i++)
  {
    fwrite(&chaine_recue[i], 1, sizeof(char), fichier);
  }

  fclose(fichier);

  // on envoie la réponse au client
  write(serviceSocket, reponse, strlen(reponse)+1);
  */
  // on ferme les sockets
  close(serviceSocket);
  close(listenSocket);

  return 0;
}
