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

int idQ;
MYSQL* connexion;
size_t taille_msg = sizeof(MESSAGE) - sizeof(long);

int main(int argc,char* argv[])
{
  // Masquage de SIGINT
  sigset_t mask;
  sigaddset(&mask,SIGINT);
  sigprocmask(SIG_SETMASK,&mask,NULL);

  // Recuperation de l'identifiant de la file de messages
  fprintf(stderr,"(ACCESBD %d) Recuperation de l'id de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,0)) == -1){perror("(ACCESBD) Erreur de msgget");exit(1);}

  // Récupération descripteur lecture du pipe
  int fdRpipe = atoi(argv[1]);

  // Connexion à la base de donnée
  connexion = mysql_init(NULL);
  if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL){
    fprintf(stderr,"(ACCESBD %d) Erreur de connexion à la base de données...\n", getpid());
    exit(1);  
  }

  MESSAGE m;
  MESSAGE reponse;
  
  char requete[200];
  MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;
  int ret;
  while(1)
  {
    // Lecture d'une requete sur le pipe
    ret = read(fdRpipe, &m, sizeof(MESSAGE));
    if(!ret){
      mysql_close(connexion);
      exit(0);
    }

    switch(m.requete)
    {
      case CONSULT :  // TO DO
                      fprintf(stderr,"(ACCESBD %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
                      // Acces BD
                      sprintf(requete, "select * from UNIX_FINAL where id = '%d'", m.data1);
                      mysql_query(connexion, requete);
                      resultat = mysql_store_result(connexion);
                      Tuple = mysql_fetch_row(resultat);
                      
                      
                      // Preparation de la reponse
                      reponse.type = m.expediteur;
                      reponse.requete = CONSULT;
                      reponse.expediteur = getpid();
                      
                      if(!Tuple)
                        reponse.data1 = -1;
                      else{
                        reponse.data1 = atoi(Tuple[0]);
                        strcpy(reponse.data2, Tuple[1]);
                        strcpy(reponse.data3, Tuple[3]);
                        strcpy(reponse.data4, Tuple[4]);
                        reponse.data5 = atof(Tuple[2]);
                      }

                      // Envoi de la reponse au bon caddie
                      if (msgsnd(idQ, &reponse, taille_msg, 0) == -1){perror("Erreur de msgsnd");exit(1);}
                      break;

      case ACHAT :    // TO DO
                      fprintf(stderr,"(ACCESBD %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
                      // Acces BD

                      // Finalisation et envoi de la reponse
                      break;

      case CANCEL :   // TO DO
                      fprintf(stderr,"(ACCESBD %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
                      // Acces BD

                      // Mise à jour du stock en BD
                      break;

    }
  }
}
