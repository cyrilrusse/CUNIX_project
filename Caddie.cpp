#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <mysql.h>
#include "protocole.h" // contient la cle et la structure d'un message

#include <iostream>

int idQ;
size_t taille_msg = sizeof(MESSAGE) - sizeof(long);

ARTICLE articles[10];
int nbArticles = 0;

int fdWpipe;
int pidClient;

MYSQL* connexion;

void handlerSIGALRM(int sig);

int main(int argc,char* argv[])
{
  // Masquage de SIGINT
  sigset_t mask;
  sigaddset(&mask,SIGINT);
  sigprocmask(SIG_SETMASK,&mask,NULL);

  // Armement des signaux
  // TO DO

  // Recuperation de l'identifiant de la file de messages
  fprintf(stderr,"(CADDIE %d) Recuperation de l'id de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,0)) == -1){perror("(CADDIE) Erreur de msgget");exit(1);}

  MESSAGE m;
  MESSAGE reponse;
  
  char requete[200];
  char newUser[20];
  MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;

  // Récupération descripteur écriture du pipe
  fdWpipe = atoi(argv[1]);

  while(1)
  {
    
    if (msgrcv(idQ,&m,taille_msg,getpid(),0) == -1){perror("(CADDIE) Erreur de msgrcv");exit(1);}

    switch(m.requete)
    {
      case LOGIN :    
                      fprintf(stderr,"(CADDIE %d) Requete LOGIN reçue de %d\n",getpid(),m.expediteur);
                      pidClient = m.expediteur;
                      break;

      case LOGOUT :   
                      fprintf(stderr,"(CADDIE %d) Requete LOGOUT reçue de %d\n",getpid(),m.expediteur);
                      exit(0);
                      break;

      case CONSULT :  
                      fprintf(stderr,"(CADDIE %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
                      m.expediteur = getpid();
                      //Transfert de la requête à AccesBD via le pipe
                      if(write(fdWpipe, &m, sizeof(MESSAGE)) != sizeof(MESSAGE)){perror("(CADDIE) Erreur de write");exit(1);}
                      //Attente de la réponse
                      fprintf(stderr,"(CADDIE %d) Attente de la réponse de BD\n", getpid());
                      if (msgrcv(idQ,&m,taille_msg,getpid(),0) == -1){perror("(CADDIE) Erreur de msgrcv");exit(1);}
                      if(m.data1 == -1)//Article non trouvé => pas de réponse
                        break;

                      //envoie de l'article au Client
                      m.type = pidClient;
                      m.expediteur = getpid();
                      fprintf(stderr, "(CADDIE %d)Envoie de la réponse au Client\n", getpid());
                      if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("(CADDIE) Erreur de msgsnd");exit(1);}
                      kill(pidClient, SIGUSR1);
                      break;

      case ACHAT :    // TO DO
                      fprintf(stderr,"(CADDIE %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);

                      // on transfert la requete à AccesBD
                      
                      // on attend la réponse venant de AccesBD
                        
                      // Envoi de la reponse au client

                      break;

      case CADDIE :   // TO DO
                      fprintf(stderr,"(CADDIE %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CANCEL :   // TO DO
                      fprintf(stderr,"(CADDIE %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);

                      // on transmet la requete à AccesBD

                      // Suppression de l'aricle du panier
                      break;

      case CANCEL_ALL : // TO DO
                      fprintf(stderr,"(CADDIE %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);

                      // On envoie a AccesBD autant de requeres CANCEL qu'il y a d'articles dans le panier

                      // On vide le panier
                      break;

      case PAYER :    // TO DO
                      fprintf(stderr,"(CADDIE %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);

                      // On vide le panier
                      break;
    }
  }
}

void handlerSIGALRM(int sig)
{
  fprintf(stderr,"(CADDIE %d) Time Out !!!\n",getpid());

  // Annulation du caddie et mise à jour de la BD
  // On envoie a AccesBD autant de requetes CANCEL qu'il y a d'articles dans le panier

  // Envoi d'un Time Out au client (s'il existe toujours)
         
  exit(0);
}