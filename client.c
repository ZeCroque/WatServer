#include "common.h"

int main(int argc, char** argv)
{
  // identification socket d'écoute du serveur
  static struct sockaddr_in addr_serveur;
  // identifiants de la machine où tourne le serveur
  struct hostent *host_serveur;
  // socket locale coté client
  int sock;
  // chaîne où sera écrit le message reçu
  char reponse[TAILLEBUF];
  // nombre d'octets envoyés/reçus
  int nb_octets;

  char* hostName="localhost";

  int port=7777;

  if(argc==3)
  {
    hostName=argv[1];
    port=strtol(argv[2], NULL, 10);
  }
  else
  {
    exit(-1);
  }

  // création socket TCP
  sock=createTCPSocket(0);

  // récupération identifiants de la machine serveur
  host_serveur = gethostbyname(hostName);
  if (host_serveur==NULL)
  {
    perror("erreur récupération adresse serveur\n");
    exit(1);
  }

  // création de l'identifiant de la socket d'écoute du serveur
  bzero((char *) &addr_serveur, sizeof(addr_serveur));
  addr_serveur.sin_family = AF_INET;
  addr_serveur.sin_port = htons(port);
  memcpy(&addr_serveur.sin_addr.s_addr, host_serveur->h_addr, host_serveur->h_length);

  // connexion de la socket client locale à la socket coté serveur
  if (connect(sock, (struct sockaddr *)&addr_serveur, sizeof(struct sockaddr_in)) == -1)
  {
    perror("erreur connexion serveur");
    exit(1);
  }

  FILE* fichier = NULL;
  long lSize=0;
  char* chaine;

  fichier = fopen("doge.jpg", "rb");

  if (fichier==NULL)
  {
    fputs ("File error",stderr);
    exit (1);
  }

   // obtain file size:
   fseek (fichier, 0, SEEK_END);
   lSize = ftell (fichier);
   rewind (fichier);
   // allocate memory to contain the whole file:
   chaine = (char*) malloc (sizeof(char)*lSize);
   if (chaine == NULL)
   {
     fputs ("Memory error",stderr);
     exit (2);
   }
   //read file
   fread(chaine, sizeof(char), lSize, fichier);



  // connexion etablie, on envoie la taille de l'image
  nb_octets = write(sock, &lSize, sizeof(long));
  printf("taille envoyée :%ld\n",lSize);

  //on envoie l'image
  nb_octets = write(sock, chaine, lSize);
  free (chaine);
  //on attend la réponse du serveur
  nb_octets = read(sock, reponse, TAILLEBUF);
  printf(" reponse recue : %s\n", reponse);
  // on ferme la socket
  close(sock);
  return 0;
}
