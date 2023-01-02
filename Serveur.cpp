#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>
#include <errno.h>
#include "protocole.h" // contient la cle et la structure d'un message
#include "FichierClient.h" // module de gestion des clients
#include "Semaphores.h"

int idQ,idShm,idSem;
int fdPipe[2];
TAB_CONNEXIONS *tab;
sigjmp_buf contexte;
size_t taille_msg = sizeof(MESSAGE) - sizeof(long);

void afficheTab();

void handlerSIGINT(int sig);
void handlerSIGCHLD(int sig);

int rechercheTableConnexion(int pid);
int rechercheCaddieTableConnexion(int pid);

int main()
{
  int index_tab;
  bool initialisation[6] = {false, false, false, false, false, false};
  // Armement des signaux

  struct sigaction B;
  B.sa_handler = handlerSIGINT;
  B.sa_flags = 0;
  sigemptyset(&B.sa_mask);
  sigaction(SIGINT, &B, NULL);

  struct sigaction C;
  C.sa_handler = handlerSIGCHLD;
  C.sa_flags = 0;
  sigemptyset(&C.sa_mask);
  sigaction(SIGCHLD, &C, NULL);

  // Création des ressources
  // Création de la file de message
  fprintf(stderr,"(SERVEUR %d) Création de la file de messages\n",getpid());
  // CLE definie dans protocole.h
  if ((idQ = msgget(CLE,IPC_CREAT | IPC_EXCL | 0600)) == -1){perror("(SERVEUR) Erreur de msgget");exit(EXIT_FAILURE);}

  // TO BE CONTINUED

  // Création du pipe
  fprintf(stderr,"(SERVEUR %d) Création du pipe\n",getpid());
  if(pipe(fdPipe)){perror("Erreur de pipe");exit(EXIT_FAILURE);}
  int flags = fcntl(fdPipe[0], F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fdPipe[0], F_SETFL, flags);

  //Création du sémaphore
  fprintf(stderr,"(SERVEUR %d) Création du sémaphore\n",getpid());
  if((idSem = semget(CLE, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1){perror("(SERVEUR) Erreur de semget");exit(EXIT_FAILURE);}

  if(semctl(idSem, 0, SETVAL, 1) == -1){perror("(SERVEUR) Erreur de semctl");exit(EXIT_FAILURE);}

  // Initialisation du tableau de connexions
  tab = (TAB_CONNEXIONS*) malloc(sizeof(TAB_CONNEXIONS));

  for (int i=0 ; i<6 ; i++){
    tab->connexions[i].pidFenetre = 0;
    strcpy(tab->connexions[i].nom,"");
    tab->connexions[i].pidCaddie = 0;
  }
  tab->pidServeur = getpid();
  tab->pidPublicite = 0;

  afficheTab();

  // Création du processus Publicite (étape 2)

  if((idShm = shmget(CLE, 52, IPC_CREAT | IPC_EXCL | 0600)) == -1){perror("(SERVEUR) Erreur de shmget");exit(EXIT_FAILURE);}
  
  tab->pidPublicite = fork();
  
  if(tab->pidPublicite == -1){perror("Erreur de fork");exit(EXIT_FAILURE);}
  else if(tab->pidPublicite == 0){
    char str[10];
    sprintf(str, "%d", idShm);
    if(execl("./Publicite", "Publicite", str, NULL) == -1){perror("Erreur de execl");exit(EXIT_FAILURE);}
  }

  // Création du processus AccesBD (étape 4)
  tab->pidAccesBD = fork();
  if(tab->pidAccesBD == -1){perror("Erreur de fork");exit(EXIT_FAILURE);}
  else if(tab->pidAccesBD == 0){
    close(fdPipe[1]);
    char str[10];
    sprintf(str, "%d", fdPipe[0]);
    if (execl("./AccesBD", "AccesBD", str, NULL) == -1){perror("Erreur de execl");exit(EXIT_FAILURE);}
  }

  MESSAGE m;
  MESSAGE reponse;
  
  while(true)
  {
  	fprintf(stderr,"(SERVEUR %d) Attente d'une requete...\n",getpid());
    // Sauvegarde du contexte, définissant où reprendre en cas d'interuption par un signal
    sigsetjmp(contexte, 1);
    if (msgrcv(idQ, &m, taille_msg, 1, 0) == -1){
      perror("(SERVEUR) Erreur de msgrcv");
      msgctl(idQ, IPC_RMID, NULL);
      exit(EXIT_FAILURE);
    }

    switch(m.requete)
    {
      case CONNECT :
              fprintf(stderr, "(SERVEUR %d) Requete CONNECT reçue de %d\n", getpid(), m.expediteur);
              
              for(int i = 0; i<6; i++){
                if(tab->connexions[i].pidFenetre == 0){
                  tab->connexions[i].pidFenetre = m.expediteur;
                  break;
                }
              }
              break;

      case DECONNECT :
              fprintf(stderr,"(SERVEUR %d) Requete DECONNECT reçue de %d\n",getpid(),m.expediteur);
              
              if((index_tab = rechercheTableConnexion(m.expediteur)) != -1)
                tab->connexions[index_tab].pidFenetre = 0;
              break;
      case LOGIN :
              fprintf(stderr,"(SERVEUR %d) Requete LOGIN reçue de %d : --%d--%s--%s--\n",getpid(),m.expediteur,m.data1,m.data2,m.data3);
              
              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              int ret;
              if((ret = sem_wait(idSem, true)) == -1){
                reponse.type = m.expediteur;
                m.requete = 0;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                kill(m.expediteur, SIGUSR1);
                break;
              }
              fprintf(stderr, "valeur retour wait : %d\n", ret);

              reponse.data1 = 1;

              // Nouveau Client
              if(m.data1 == 1){
                if(estPresent(m.data2)){
                  reponse.data1 = 0;
                  strcpy(reponse.data4, "Nouveau Client impossible : Client existe déjà");
                }
                else{
                  ajouteClient(m.data2, m.data3);
                }
              }
              // Connexion Client déjà existant
              else{
                int pos = estPresent(m.data2);
                if(!pos){
                  reponse.data1 = 0;
                  strcpy(reponse.data4, "Tentative de connexion à un client inexistant");
                }
                else if(!verifieMotDePasse(pos, m.data3)){
                  reponse.data1 = 0;
                  strcpy(reponse.data4, "Mot de passe incorrect");
                }
              }

              // LOGIN success
              if(reponse.data1){
                index_tab = rechercheTableConnexion(m.expediteur);
                // Vérifie que le client qui veut se LOGIN est connecté au serveur
                if(index_tab != -1)
                  strcpy(tab->connexions[index_tab].nom, m.data2);

                pid_t pid_caddie = fork();
                //Si erreur de fork, annule le LOGIN
                if(pid_caddie == -1){//Erreur fork
                  perror("Erreur de fork");
                  reponse.data1 = 0;
                  strcpy(reponse.data4, "Impossible de créer le Caddie");
                  strcpy(tab->connexions[index_tab].nom, "");
                }
                else if(pid_caddie == 0){//Caddie
                  close(fdPipe[0]);
                  char str_fdpipe[10];
                  sprintf(str_fdpipe, "%d", fdPipe[1]);
                  if (execl("./Caddie", "Caddie", str_fdpipe, NULL) == -1){perror("Erreur de execl()");exit(EXIT_FAILURE);}
                }
                else{//Serveur -> stock pid Caddie
                  tab->connexions[index_tab].pidCaddie = pid_caddie;
                  // Retransmet la requête LOGIN au nouveau Caddie
                  m.type = pid_caddie;
                  if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                }
              }

              //Envoi du feedback de login au client
              reponse.type = m.expediteur;
              reponse.expediteur = getpid();
              reponse.requete = LOGIN;

              if (msgsnd(idQ, &reponse, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
              kill(m.expediteur, SIGUSR1);
              
              // Libération du semaphore
              sem_signal(idSem);
              break;

      case LOGOUT :
              fprintf(stderr,"(SERVEUR %d) Requete LOGOUT reçue de %d\n",getpid(), m.expediteur);

              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              if(sem_wait(idSem, true) == -1){
                reponse.type = m.expediteur;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                break;
              }

              if((index_tab = rechercheTableConnexion(m.expediteur))!= -1){
                strcpy(tab->connexions[index_tab].nom, "");
                m.type = tab->connexions[index_tab].pidCaddie;
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){
                  perror("Erreur de msgsnd");
                  exit(EXIT_FAILURE);
                }
                tab->connexions[index_tab].pidCaddie = 0;
              }

              // Libération du semaphore
              sem_signal(idSem);
              break;

      case UPDATE_PUB : 
              fprintf(stderr,"(SERVEUR %d) Requete UPDATE_PUB reçue de %d\n",getpid(), m.expediteur);
              
              for(int i = 0; i<6; i++){
                if(tab->connexions[i].pidFenetre != 0){
                  if(initialisation[i])
                    kill(tab->connexions[i].pidFenetre, SIGUSR2);
                  else
                    initialisation[i] = true;
                }
              }

              break;

      case CONSULT :  
              fprintf(stderr,"(SERVEUR %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
              
              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              if(sem_wait(idSem, true) == -1){
                reponse.type = m.expediteur;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                break;
              }              

              index_tab = rechercheTableConnexion(m.expediteur);
          
              if(index_tab != -1){
                m.type = tab->connexions[index_tab].pidCaddie;

                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
              }

              // Libération du semaphore
              sem_signal(idSem);
              break;

      case ACHAT :    
              fprintf(stderr,"(SERVEUR %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
              
              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              if(sem_wait(idSem, true) == -1){
                reponse.type = m.expediteur;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                break;
              }

              index_tab = rechercheTableConnexion(m.expediteur);
              if(index_tab != -1){
                m.type = tab->connexions[index_tab].pidCaddie;
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
              }

              // Libération du semaphore
              sem_signal(idSem);
              break;

      case CADDIE :
              fprintf(stderr,"(SERVEUR %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
              
              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              if(sem_wait(idSem, true) == -1){
                reponse.type = m.expediteur;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                break;
              }
              
              index_tab = rechercheTableConnexion(m.expediteur);
              if(index_tab != -1){
                m.type = tab->connexions[index_tab].pidCaddie;
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
              }

              // Libération du semaphore
              sem_signal(idSem);
              break;

      case CANCEL :
              fprintf(stderr,"(SERVEUR %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
              
              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              if(sem_wait(idSem, true) == -1){
                reponse.type = m.expediteur;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                break;
              }

              index_tab = rechercheTableConnexion(m.expediteur);
              if(index_tab != -1){
                m.type = tab->connexions[index_tab].pidCaddie;
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
              }

              // Libération du semaphore
              sem_signal(idSem);    
              break;

      case CANCEL_ALL :
              fprintf(stderr,"(SERVEUR %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);
              
              //Acquisition du sémaphore en no wait, donc si erreur, envoi d'un message busy au client
              if(sem_wait(idSem, true) == -1){
                reponse.type = m.expediteur;
                reponse.requete = BUSY;
                reponse.expediteur = getpid();
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
                break;
              }

              index_tab = rechercheTableConnexion(m.expediteur);
              if(index_tab != -1){
                m.type = tab->connexions[index_tab].pidCaddie;
                if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(EXIT_FAILURE);}
              }

              // Libération du semaphore
              sem_signal(idSem);    
              break;

      case PAYER : // TO DO
              fprintf(stderr,"(SERVEUR %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);
              
              // Libération du semaphore
              // sem_signal(idSem);
              break;

      case NEW_PUB :  // TO DO
              fprintf(stderr,"(SERVEUR %d) Requete NEW_PUB reçue de %d\n",getpid(),m.expediteur);
              
              break;
    }
    afficheTab();
  }
}

void afficheTab()
{
  fprintf(stderr,"Pid Serveur  : %d\n",tab->pidServeur);
  fprintf(stderr,"Pid Publicite : %d\n",tab->pidPublicite);
  fprintf(stderr,"Pid AccesBD  : %d\n",tab->pidAccesBD);
  for (int i=0 ; i<6 ; i++)
    fprintf(stderr,"%6d -%20s- %6d\n",tab->connexions[i].pidFenetre,
                                                      tab->connexions[i].nom,
                                                      tab->connexions[i].pidCaddie);
  fprintf(stderr,"\n");
}

////////////////////////////////////////////////////////////////////////////////////

void handlerSIGINT(int sig){
  errno = 0;
  // Supprime la file de messages
  if (msgctl(idQ, IPC_RMID, NULL) == -1)
    perror("(SERVEUR) Erreur de msgctl");

  // Supprime la mémoire partagée
  if (shmctl(idShm, IPC_RMID, NULL) == -1)
    perror("(SERVEUR) Erreur de shmctl");

  // Ferme les descripteurs du pipe sur le serveur
  if(close(fdPipe[0]) == -1)
    perror("(SERVEUR) Erreur de close");
  if(close(fdPipe[1]) == -1)
    perror("(SERVEUR) Erreur de close");

  // Supprime le sémaphore
  if (semctl(idSem, 0, IPC_RMID) == -1)
    perror("(SERVEUR) Erreur de semctl");

  if(errno)
    exit(EXIT_FAILURE);
  
  exit(EXIT_SUCCESS);
}

void handlerSIGCHLD(int sig){
  int id = wait(NULL);
  fprintf(stderr, "(SERVEUR %d) Suppression du fils zombi %d\n", getpid(), id);

  int index_tab = rechercheCaddieTableConnexion(id);
  if(index_tab != -1)
    tab->connexions[index_tab].pidCaddie = 0;

  siglongjmp(contexte, 2);
}

int rechercheTableConnexion(int pid){
  for(int i=0; i<6; i++){
    if(tab->connexions[i].pidFenetre == pid)
      return i;
  }

  return -1;
}

int rechercheCaddieTableConnexion(int pid){
  for (int i = 0; i < 6; i++){
    if (tab->connexions[i].pidCaddie == pid)
      return i;
  }

  return -1;
}
