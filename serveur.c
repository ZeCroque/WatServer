#include "common.h"

int main(int argc, char** argv)
{
  // adresse socket coté client
  static struct sockaddr_in addr_client;
  // longueur adresse
  int lg_addr;
  // socket d'écoute et de service
  int socket_ecoute, socket_service;
  // chaîne reçue du client
  char *chaine_recue;
  // chaîne renvoyée au client
  char *reponse = "bien recu";
  // nombre d'octets reçus ou envoyés
  int nb_octets;

  long lSize;

  int port=7777;

  if(argc==2)
  {
    port=strtol(argv[1], NULL, 10);
  }
  else
  {
    exit(-1);
  }

  // création socket TCP d'écoute
  socket_ecoute=creerSocketTCP(port);


  // configuration socket écoute : 5 connexions max en attente
  if (listen(socket_ecoute, 5) == -1)
  {
    perror("erreur listen");
    exit(1);
  }

  // on attend la connexion du client
  lg_addr = sizeof(struct sockaddr_in);
  socket_service = accept(socket_ecoute, (struct sockaddr *)&addr_client, (socklen_t *)&lg_addr);
  if (socket_service == -1)
  {
    perror("erreur accept");
    exit(1);
  }

  // la connexion est établie, on attend les données envoyées par le client
  nb_octets = read(socket_service, &lSize, sizeof(long));
  printf("taille reçue :%ld\n",lSize);
  chaine_recue =(char *)malloc(lSize * sizeof(char));
  nb_octets = read(socket_service, chaine_recue, lSize);
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
  write(socket_service, reponse, strlen(reponse)+1);

  // on ferme les sockets
  close(socket_service);
  close(socket_ecoute);

  return 0;
}
