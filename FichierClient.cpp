#include "FichierClient.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


int estPresent(const char* nom){
   // Check if the client file exists
   if (access(FICHIER_CLIENTS, F_OK) == -1){
      printf("fichier n'existe pas\n");
      return 0;
   }

   // Open the client file in read only mode
   int fd = open(FICHIER_CLIENTS, O_RDONLY);
   if (fd == -1){
      perror("Erreur de open :");
      exit(-1);
   }

   
   CLIENT clientToCheck;
   int nbrRead = 0, position = 1, name_length = 1;
   errno = 0;

   // Compute name size, to know how many bytes we have to compare
   while (nom[name_length]!='\0')
      name_length++;

   // Read sequentialy until it finds the client with the name "nom"
   do{
      if ((nbrRead = read(fd, &clientToCheck, sizeof(CLIENT))) == -1){
         if(errno)
            perror("Erreur de read :");
      }
      
         
      //If client found, return the position of the client in the file
      //and close the client file
      
      if(!strncmp(clientToCheck.nom, nom, name_length)){
         if (close(fd) == -1){
            perror("Erreur de close :");
            exit(-1);
         }
         return position;
      }
      position ++;
   }while(nbrRead == sizeof(CLIENT));

   //Close the client file
   if (close(fd) == -1){
      perror("Erreur de close :");
      exit(-1);
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////////
int hash(const char* motDePasse){
   int i = 0, hash = 0;
   //Compute password hash
   while(motDePasse[i] != '\0'){
      hash += motDePasse[i] * i;
      i++;
   }

   hash %= 97;

   return hash;
}

////////////////////////////////////////////////////////////////////////////////////
void ajouteClient(const char* nom, const char* motDePasse){
   //Create struct of the new client to save in the client file
   CLIENT newClient;
   strcpy(newClient.nom, nom);
   newClient.hash = hash(motDePasse);

   //Open client file
   int fd = open(FICHIER_CLIENTS, O_CREAT|O_WRONLY|O_APPEND);
   if(fd == -1){
      perror("Erreur de open :");
      exit(-1);
   }
   
   if(chmod(FICHIER_CLIENTS, 0666)){
      perror("Erreur de chmod()");
      exit(-1);
   }

   //Write the new client in the client file
   if(write(fd, &newClient, sizeof(CLIENT)) == -1){
      perror("Erreur de write :");
      if(close(fd) == -1)
         perror("Erreur de close :");
      exit(-1);
   }

   //Close the client file
   if(close(fd) == -1){
      perror("Erreur de close :");
      exit(-1);
   }
}

////////////////////////////////////////////////////////////////////////////////////
int verifieMotDePasse(int pos, const char* motDePasse){
   errno = 0;

   //Check if the client file exists
   if(access(FICHIER_CLIENTS, F_OK) == -1){
      if(errno){
         perror("Erreur lors de la vérification d'existence du fichier client :");
         exit(-1);
      }
      return -1;
   }

   //Open the client file in read only mode
   int fd = open(FICHIER_CLIENTS, O_RDONLY);
   if(fd == -1){
      perror("Erreur de open :");
      exit(-1);
   }

   //Set the seek at the willen position
   if(lseek(fd, (pos-1)*sizeof(CLIENT), SEEK_SET) == -1){
      perror("Erreur de lseek :");
      exit(-1);
   }

   //Read the client at the position where the seek has been set
   CLIENT clientToCheck;
   if(read(fd, &clientToCheck, sizeof(CLIENT)) == -1){
      perror("Erreur de read :");
      exit(-1);
   }

   //Close the client file
   if(close(fd) == -1){
      perror("Erreur de close :");
      exit(-1);
   }

   //Check if the client hash correspond to the given password
   if(clientToCheck.hash == hash(motDePasse))
      return 1;
   
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////
int listeClients(CLIENT *vecteur){ // le vecteur doit etre suffisamment grand

   // Check if the client file exists
   if (access(FICHIER_CLIENTS, F_OK) == -1){
      return -1;
   }

   // Open the client file in read only mode
   int fd = open(FICHIER_CLIENTS, O_RDONLY);
   if (fd == -1){
      perror("Erreur de open :");
      exit(-1);
   }

   // Read sequentialy until it finds the client with the name "nom"
   int nbrRead = 0, position = 0;
   errno = 0;
   do{
      if ((nbrRead = read(fd, &vecteur[position], sizeof(CLIENT))) == -1){
         if (errno){
            perror("Erreur de read :");
            if (close(fd) == -1){
               perror("Erreur de close :");
               exit(-1);
            }
            exit(-1);
         }
      }
      position++;
   } while (nbrRead == sizeof(CLIENT));

   // Close the client file
   if (close(fd) == -1){
      perror("Erreur de close :");
      exit(-1);
   }

   return position - 1;
   }
