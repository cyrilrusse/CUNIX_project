#include "windowgerant.h"
#include "ui_windowgerant.h"
#include <iostream>
using namespace std;
#include <mysql.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string>
#include "protocole.h"
#include "Semaphores.h"

int idArticleSelectionne = -1;
MYSQL *connexion;
MYSQL_RES  *resultat;
MYSQL_ROW  Tuple;
char requete[200];
int idSem;
int idQ;
size_t taille_msg = sizeof(MESSAGE) - sizeof(long);

WindowGerant::WindowGerant(QWidget *parent) : QMainWindow(parent),ui(new Ui::WindowGerant){
  ui->setupUi(this);

  // Configuration de la table du stock (ne pas modifer)
  ui->tableWidgetStock->setColumnCount(4);
  ui->tableWidgetStock->setRowCount(0);
  QStringList labelsTableStock;
  labelsTableStock << "Id" << "Article" << "Prix à l'unité" << "Quantité";
  ui->tableWidgetStock->setHorizontalHeaderLabels(labelsTableStock);
  ui->tableWidgetStock->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableWidgetStock->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableWidgetStock->horizontalHeader()->setVisible(true);
  ui->tableWidgetStock->horizontalHeader()->setDefaultSectionSize(120);
  ui->tableWidgetStock->horizontalHeader()->setStretchLastSection(true);
  ui->tableWidgetStock->verticalHeader()->setVisible(false);
  ui->tableWidgetStock->horizontalHeader()->setStyleSheet("background-color: lightyellow");

  // Recuperation de la file de message
  if ((idQ = msgget(CLE,0)) == -1){perror("(GERANT) Erreur de msgget");exit(EXIT_FAILURE);}

  // Récupération du sémaphore
  if ((idSem = semget(CLE, 0, 0)) == -1){perror("(GERANT) Erreur de semget");exit(EXIT_FAILURE);}

  // Prise blocante du semaphore
  if(sem_wait(idSem, false) == -1){perror("(GERANT) Erreur lors de l'obtention du sémaphore");exit(EXIT_FAILURE);}

  // Connexion à la base de donnée
  connexion = mysql_init(NULL);
  fprintf(stderr,"(GERANT %d) Connexion à la BD\n",getpid());
  if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL){
    fprintf(stderr,"(GERANT %d) Erreur de connexion à la base de données...\n",getpid());
    exit(EXIT_FAILURE);  
  }

  // Recuperation des articles en BD
  sprintf(requete, "select * from UNIX_FINAL");
  mysql_query(connexion, requete);
  resultat = mysql_store_result(connexion);
  while((Tuple = mysql_fetch_row(resultat)) != 0){
    string tmp(Tuple[2]);
    size_t x = tmp.find(".");
    if (x != string::npos) tmp.replace(x,1,",");
    ajouteArticleTablePanier(atoi(Tuple[0]), Tuple[1], std::stof(tmp), atoi(Tuple[3]));
  }
}

WindowGerant::~WindowGerant(){
  delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table du stock (ne pas modifier) //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowGerant::ajouteArticleTablePanier(int id,const char* article,float prix,int quantite){
  char Id[20],Prix[20],Quantite[20];

  sprintf(Id,"%d",id);
  sprintf(Prix,"%.2f",prix);
  sprintf(Quantite,"%d",quantite);

  // Ajout possible
  int nbLignes = ui->tableWidgetStock->rowCount();
  nbLignes++;
  ui->tableWidgetStock->setRowCount(nbLignes);
  ui->tableWidgetStock->setRowHeight(nbLignes-1,10);

  QTableWidgetItem *item = new QTableWidgetItem;
  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  item->setTextAlignment(Qt::AlignCenter);
  item->setText(Id);
  ui->tableWidgetStock->setItem(nbLignes-1,0,item);

  item = new QTableWidgetItem;
  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  item->setTextAlignment(Qt::AlignCenter);
  item->setText(article);
  ui->tableWidgetStock->setItem(nbLignes-1,1,item);

  item = new QTableWidgetItem;
  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  item->setTextAlignment(Qt::AlignCenter);
  item->setText(Prix);
  ui->tableWidgetStock->setItem(nbLignes-1,2,item);

  item = new QTableWidgetItem;
  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  item->setTextAlignment(Qt::AlignCenter);
  item->setText(Quantite);
  ui->tableWidgetStock->setItem(nbLignes-1,3,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowGerant::videTableStock(){
  ui->tableWidgetStock->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowGerant::getIndiceArticleSelectionne(){
  QModelIndexList liste = ui->tableWidgetStock->selectionModel()->selectedRows();
  if (liste.size() == 0) return -1;
  QModelIndex index = liste.at(0);
  int indice = index.row();
  return indice;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowGerant::on_tableWidgetStock_cellClicked(int row, int column){
  //cerr << "ligne=" << row << " colonne=" << column << endl;
  ui->lineEditIntitule->setText(ui->tableWidgetStock->item(row,1)->text());
  ui->lineEditPrix->setText(ui->tableWidgetStock->item(row,2)->text());
  ui->lineEditStock->setText(ui->tableWidgetStock->item(row,3)->text());
  idArticleSelectionne = atoi(ui->tableWidgetStock->item(row,0)->text().toStdString().c_str());
  //cerr << "id = " << idArticleSelectionne << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float WindowGerant::getPrix(){
  return atof(ui->lineEditPrix->text().toStdString().c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowGerant::getStock(){
  return atoi(ui->lineEditStock->text().toStdString().c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowGerant::getPublicite(){
  strcpy(publicite,ui->lineEditPublicite->text().toStdString().c_str());
  return publicite;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// CLIC SUR LA CROIX DE LA FENETRE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowGerant::closeEvent(QCloseEvent *event){
  fprintf(stderr,"(GERANT %d) Clic sur croix de la fenetre\n",getpid());

  // Deconnexion BD
  mysql_close(connexion);

  // Libération du semaphore
  if(sem_signal(idSem) == -1){perror("(GERANT) Erreur lors de la libération du sémaphore");exit(EXIT_FAILURE);}

  exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowGerant::on_pushButtonPublicite_clicked(){
  fprintf(stderr,"(GERANT %d) Clic sur bouton Mettre a jour\n",getpid());
  // (étape 7)
  // Envoi d'une requete NEW_PUB au serveur

  MESSAGE m;
  m.type = 1;
  m.requete = NEW_PUB;
  m.expediteur = getpid();
  strcpy(m.data4, getPublicite());

  if (msgsnd(idQ, &m, taille_msg, 0) == -1){perror("(GERANT) Erreur de msgsnd");exit(EXIT_FAILURE);}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowGerant::on_pushButtonModifier_clicked(){
  fprintf(stderr,"(GERANT %d) Clic sur bouton Modifier\n",getpid());

  char Prix[20];
  sprintf(Prix,"%f",getPrix());

  fprintf(stderr,"(GERANT %d) Modification en base de données pour id=%d\n",getpid(),idArticleSelectionne);

  // Mise a jour table BD

  // Remplace les virgules
  string tmp(Prix);
  size_t x = tmp.find(",");
  if(x != string::npos) tmp.replace(x, 1, ".");

  sprintf(requete, "update UNIX_FINAL set prix = '%s' where id = '%d'", tmp.c_str(), idArticleSelectionne);
  mysql_query(connexion, requete);
  sprintf(requete, "update UNIX_FINAL set stock = '%d' where id = '%d'", getStock(), idArticleSelectionne);
  mysql_query(connexion, requete);

  // Recuperation des articles en BD
  sprintf(requete, "select * from UNIX_FINAL");
  mysql_query(connexion, requete);
  resultat = mysql_store_result(connexion);
  videTableStock();
  while((Tuple = mysql_fetch_row(resultat)) != 0){
    string tmp(Tuple[2]);
    size_t x = tmp.find(".");
    if (x != string::npos) tmp.replace(x,1,",");
    ajouteArticleTablePanier(atoi(Tuple[0]), Tuple[1], std::stof(tmp), atoi(Tuple[3]));
  }
}
