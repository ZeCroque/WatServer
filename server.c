#include "common.h"

int main()
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

  printf("Démarrage..");

	listenSocket=createTCPSocket(7777);
  // création socket TCP d'écoute

  	printf("socket créée !");

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
  chaine_recue =(char *)malloc(6 * sizeof(char));
  nb_octets = read(serviceSocket, chaine_recue, 6 * sizeof(char));
  printf("taille reçue:%d", nb_octets);
  printf("message:%s",chaine_recue);



  // on envoie la réponse au client
  write(serviceSocket, reponse, strlen(reponse)+1);

  // on ferme les sockets
  close(serviceSocket);
  close(listenSocket);

  return 0;
}
