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
  if ((idQ = msgget(CLE,0)) == -1){perror("(ACCESBD) Erreur de msgget");exit(EXIT_FAILURE);}

  // Récupération descripteur lecture du pipe
  int fdRpipe = atoi(argv[1]);

  // Connexion à la base de donnée
  connexion = mysql_init(NULL);
  if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL){
    fprintf(stderr,"(ACCESBD %d) Erreur de connexion à la base de données...\n", getpid());
    exit(EXIT_FAILURE);  
  }

  MESSAGE m;
  MESSAGE reponse;
  
  char requete[200];
  MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;
  int ret;
  while(true)
  {
    // Lecture d'une requete sur le pipe
    ret = read(fdRpipe, &m, sizeof(MESSAGE));
    if(ret == 0){
      mysql_close(connexion);
      exit(EXIT_SUCCESS);
    }
    else if(ret == -1)
      continue;

    switch(m.requete)
    {
      case CONSULT :  
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
              if (msgsnd(idQ, &reponse, taille_msg, 0) == -1){perror("(ACCESBD) Erreur de msgsnd");exit(EXIT_FAILURE);}
              break;

      case ACHAT : 
              fprintf(stderr,"(ACCESBD %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
              // Acces BD
              sprintf(requete, "select * from UNIX_FINAL where id = '%d'", m.data1);
              mysql_query(connexion, requete);
              resultat = mysql_store_result(connexion);
              Tuple = mysql_fetch_row(resultat);

              // Preparation de la reponse
              reponse.type = m.expediteur;
              reponse.requete = ACHAT;
              reponse.expediteur = getpid();
              reponse.data1 = atoi(Tuple[0]);
              strcpy(reponse.data2, Tuple[1]);
              
              //quantité ou 0 si pas assez de stock
              if(atoi(Tuple[3]) >= atoi(m.data2)){
                //Décremente dans la BD
                sprintf(requete, "update UNIX_FINAL set stock = stock - '%d' where id = '%d'", atoi(m.data2), m.data1);
                mysql_query(connexion, requete);
                strcpy(reponse.data3, m.data2);
              }
              else
                strcpy(reponse.data3, "0");

              strcpy(reponse.data4, Tuple[4]);
              reponse.data5 = atof(Tuple[2]);
              
              // Envoi de la reponse
              if (msgsnd(idQ, &reponse, taille_msg, 0) == -1){perror("(ACCESBD) Erreur de msgsnd");exit(EXIT_FAILURE);}

              break;

      case CANCEL : 
              fprintf(stderr,"(ACCESBD %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
              // Acces BD

              // Mise à jour du stock en BD
              sprintf(requete, "update UNIX_FINAL set stock = stock + '%d' where id = '%d'", atoi(m.data2), m.data1);
              mysql_query(connexion, requete);
              break;

    }
  }
}
