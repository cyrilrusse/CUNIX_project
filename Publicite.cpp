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
#include <errno.h>
#include <setjmp.h>
#include "protocole.h" // contient la cle et la structure d'un message

int idQ, idShm;
char *pShm;
void handlerSIGUSR1(int sig);
int fd;
size_t taille_msg = sizeof(MESSAGE) - sizeof(long);
sigjmp_buf contexte;

int main(int argc, char* argv[])
{
  // Armement des signaux
  struct sigaction A;
  A.sa_handler = handlerSIGUSR1;
  A.sa_flags = 0;
  sigemptyset(&A.sa_mask);
  sigaction(SIGUSR1, &A, NULL);

  // Masquage des signaux
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask,SIGUSR1);
  sigprocmask(SIG_SETMASK,&mask,NULL);

  // Recuperation de l'identifiant de la file de messages
  fprintf(stderr,"(PUBLICITE %d) Recuperation de l'id de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,0)) == -1){perror("(PUBLICITE) Erreur de msgget");exit(EXIT_FAILURE);}

  // Recuperation de l'identifiant de la mémoire partagée
  idShm = atoi(argv[1]);
  
  // Attachement à la mémoire partagée
  if((pShm = (char*)shmat(idShm, NULL, 0)) == (char*)-1){perror("Erreur de shmat");exit(EXIT_FAILURE);}


  // Mise en place de la publicité en mémoire partagée
  char pub[51];
  strcpy(pub,"Bienvenue sur le site du Maraicher en ligne !");

  for (int i=0 ; i<=50 ; i++) pShm[i] = ' ';
  pShm[51] = '\0';
  int indDebut = 25 - strlen(pub)/2;
  for (int i=0 ; i<strlen(pub) ; i++) pShm[indDebut + i] = pub[i];

  MESSAGE m;
  sigsetjmp(contexte, 1);

  while(1)
  {
    // Envoi d'une requete UPDATE_PUB au serveur
    m.type = 1;
    m.requete = UPDATE_PUB;
    m.expediteur = getpid();

    // On part du principe que si l'erreur de msgsnd est lié à la suppression de la file 
    // de message, le serveur doit s'être terminé et l'a supprimé, donc on termine également publicite
    if (msgsnd(idQ, &m, taille_msg, 0) == -1){
      if(errno == EINVAL)
        exit(EXIT_SUCCESS); 
        
      perror("(PUBLICITE) Erreur de msgsnd");
      exit(EXIT_FAILURE);
    }

    sleep(1); 

    // Decallage vers la gauche
    char tampon = pShm[0];
    for(int i = 0; i<50; i++)
      pShm[i] = pShm[i+1];

    pShm[50] = tampon;
  }
}

void handlerSIGUSR1(int sig)
{
  fprintf(stderr,"(PUBLICITE %d) Nouvelle publicite !\n",getpid());
  MESSAGE m;

  // Lecture message NEW_PUB
  fprintf(stderr, "ok\n");
  if (msgrcv(idQ, &m, taille_msg, getpid(), 0) == -1){perror("(PUBLICITE) Erreur de msgrcv");exit(EXIT_FAILURE);}
  

  // Mise en place de la publicité en mémoire partagée
  for (int i=0 ; i<=50 ; i++) pShm[i] = ' ';
  pShm[51] = '\0';
  int indDebut = 25 - strlen(m.data4)/2;
  fprintf(stderr, "len : %d\n", strlen(m.data4));
  for (int i=0 ; i<strlen(m.data4) ; i++) pShm[indDebut + i] = m.data4[i];


  siglongjmp(contexte, 1);
}
