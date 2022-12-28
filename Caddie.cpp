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
  if ((idQ = msgget(CLE,0)) == -1){perror("(CADDIE) Erreur de msgget");exit(EXIT_FAILURE);}

  MESSAGE m;
  MESSAGE reponse;
  
  char requete[200];
  char newUser[20];
  MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;

  // Récupération descripteur écriture du pipe
  fdWpipe = atoi(argv[1]);


  int indice;
  char str[10];

  while(true)
  {
    
    if (msgrcv(idQ,&m,taille_msg,getpid(),0) == -1){perror("(CADDIE) Erreur de msgrcv");exit(EXIT_FAILURE);}

    switch(m.requete)
    {
      case LOGIN :    
              fprintf(stderr,"(CADDIE %d) Requete LOGIN reçue de %d\n",getpid(),m.expediteur);
              pidClient = m.expediteur;
              break;

      case LOGOUT :   
              fprintf(stderr,"(CADDIE %d) Requete LOGOUT reçue de %d\n",getpid(),m.expediteur);
              close(fdWpipe);
              exit(EXIT_SUCCESS);
              break;

      case CONSULT :  
              fprintf(stderr,"(CADDIE %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
              m.expediteur = getpid();
              //Transfert de la requête à AccesBD via le pipe
              if(write(fdWpipe, &m, sizeof(MESSAGE)) != sizeof(MESSAGE)){perror("(CADDIE) Erreur de write");exit(EXIT_FAILURE);}
              //Attente de la réponse
              if (msgrcv(idQ,&m,taille_msg,getpid(),0) == -1){perror("(CADDIE) Erreur de msgrcv");exit(EXIT_FAILURE);}
              if(m.data1 == -1)//Article non trouvé => pas de réponse
                break;

              //Envoi de l'article au Client
              m.type = pidClient;
              m.expediteur = getpid();
              fprintf(stderr, "(CADDIE %d)Envoie de la réponse au Client\n", getpid());
              if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("(CADDIE) Erreur de msgsnd");exit(EXIT_FAILURE);}
              kill(pidClient, SIGUSR1);
              break;

      case ACHAT :
              fprintf(stderr,"(CADDIE %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);

              // on transfert la requete à AccesBD
              m.expediteur = getpid();
              if(write(fdWpipe, &m, sizeof(MESSAGE)) != sizeof(MESSAGE)){perror("(CADDIE) Erreur de write");exit(EXIT_FAILURE);}
              
              // on attend la réponse venant de AccesBD
              if (msgrcv(idQ,&m,taille_msg,getpid(),0) == -1){perror("(CADDIE) Erreur de msgrcv");exit(EXIT_FAILURE);}
              
              // Ajout au vecteur d'articles
              if(atoi(m.data3) != 0){
                bool found = false;
                for(int i=0; i<nbArticles; i++){
                  if (articles[i].id == m.data1){
                    articles[i].stock += atoi(m.data3);
                    found = true;
                    break;
                  }
                }
                if(!found){
                  articles[nbArticles].id = m.data1;
                  strcpy(articles[nbArticles].intitule, m.data2);
                  articles[nbArticles].stock = atoi(m.data3);
                  strcpy(articles[nbArticles].image, m.data4);
                  articles[nbArticles].prix = m.data5;
                  nbArticles++;
                }
              }

              // Envoi de la reponse au client
              m.type = pidClient;
              m.expediteur = getpid();
              if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("(CADDIE) Erreur de msgsnd");exit(EXIT_FAILURE);}
              kill(pidClient, SIGUSR1);
              break;

      case CADDIE :
              fprintf(stderr,"(CADDIE %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
              
              reponse.type = pidClient;
              reponse.requete = CADDIE;
              reponse.expediteur = getpid();
              char str[10];
              for(int i=0; i<nbArticles; i++){
                reponse.data1 = articles[i].id;
                strcpy(reponse.data2, articles[i].intitule);
                sprintf(str, "%d", articles[i].stock);
                strcpy(reponse.data3, str);
                strcpy(reponse.data4, articles[i].image);
                reponse.data5 = articles[i].prix;

                if (msgsnd(idQ, &reponse, taille_msg, 0) == -1){perror("(CADDIE) Erreur de msgsnd");exit(EXIT_FAILURE);}
                if (kill(pidClient, SIGUSR1) == -1){perror("(CADDIE) Erreur de kill");exit(EXIT_FAILURE);}

              }
              
              break;

      case CANCEL :
              fprintf(stderr,"(CADDIE %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
              indice = m.data1;
              
              // on transmet la requete à AccesBD
              m.expediteur = getpid();
              m.data1 = articles[indice].id;
              sprintf(str, "%d", articles[indice].stock);
              strcpy(m.data2, str);
              if(write(fdWpipe, &m, sizeof(MESSAGE)) != sizeof(MESSAGE)){perror("(CADDIE) Erreur de write");exit(EXIT_FAILURE);}

              // Suppression de l'aricle du panier
              if((indice+1) != nbArticles){
                articles[indice].id = articles[nbArticles-1].id;
                strcpy(articles[indice].intitule, articles[nbArticles-1].intitule);
                articles[indice].prix = articles[nbArticles-1].prix;
                articles[indice].stock = articles[nbArticles-1].stock;
                strcpy(articles[indice].image, articles[nbArticles-1].image);
              }
              nbArticles--;
              break;

      case CANCEL_ALL :
              fprintf(stderr,"(CADDIE %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);
              m.requete = CANCEL;
              m.expediteur = getpid();
              // On envoie a AccesBD autant de requeres CANCEL qu'il y a d'articles dans le panier
              for(int i=0; i<nbArticles; i++){
                m.data1 = articles[i].id;
                sprintf(str, "%d", articles[i].stock);
                strcpy(m.data2, str);
                if(write(fdWpipe, &m, sizeof(MESSAGE)) != sizeof(MESSAGE)){perror("(CADDIE) Erreur de write");exit(EXIT_FAILURE);}
              }

              // On vide le panier
              nbArticles = 0;
              break;

      case PAYER :
              fprintf(stderr,"(CADDIE %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);

              // On vide le panier
              nbArticles = 0;
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
         
  exit(EXIT_SUCCESS);
}