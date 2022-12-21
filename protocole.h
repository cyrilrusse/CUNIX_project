
#define CLE 1234

//      requete             sens      data1           data2         data3          data4          data5
#define CONNECT       1  // Cl -> S
#define DECONNECT     2  // Cl -> S
#define LOGIN         3  // Cl -> S   1 ou 0          nom           mot de passe
//                          S  -> Cl  1 ou 0                                       reponse en phrase pour l'utilisateur
#define LOGOUT        4  // Cl -> S 
#define UPDATE_PUB    5  // P  -> S
#define CONSULT       6  // Cl -> S   idArticle
//                          S  -> Ca  idArticle
//                          Ca -> BD  idArticle
//                          BD -> Ca  idArticle ou -1 intitule      stock          image          prix
//                          Ca -> Cl  idArticle       intitule      stock          image          prix
#define ACHAT         7  // Cl -> S   idArticle       quantite
//                          S  -> Ca  idArticle       quantite
//                          Ca -> BD  idArticle       quantite
//                          BD -> Ca  idArticle       intitule      quantite ou 0  image          prix
//                          Ca -> Cl  idArticle       intitule      quantite ou 0  image          prix
#define CADDIE        8  // Cl -> S   
//                          S  -> Ca
//                          Ca -> Cl  idArticle       intitule      quantite       image          prix
#define CANCEL        9  // Cl -> S   indiceArticle
//                          S  -> Ca  indiceArticle
//                          Ca -> BD  idArticle       quantite
#define CANCEL_ALL   10  // Cl -> S
//                          S  -> Ca
#define PAYER        11  // Cl -> S
//                       // S  -> Ca
#define TIME_OUT     12  // Ca -> Cl
#define BUSY         13  // S  -> Cl
#define NEW_PUB      14  // Ge -> S                                                publicite
//                          S  -> P                                                publicite

typedef struct
{
  long  type;
  int   expediteur;
  int   requete;
  int   data1;
  char  data2[20];
  char  data3[20];
  char  data4[100];
  float data5;
} MESSAGE;

typedef struct
{
  int   pidFenetre;
  char  nom[20];
  int   pidCaddie;
} CONNEXION;

typedef struct
{
  int   id;
  char  intitule[20];
  float prix;
  int   stock;  
  char  image[20];
} ARTICLE;

typedef struct
{
  int   pidServeur;
  int   pidPublicite;
  int   pidAccesBD;
  CONNEXION connexions[6];
} TAB_CONNEXIONS;

