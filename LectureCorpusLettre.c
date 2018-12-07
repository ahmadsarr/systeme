#include "pere.h"
#include "semaphore.h"
  int *vGobal;
/* Fonction d'écritue de l'histogramme dans un fichier */
void ecrireFichier(char *nomFichier, long int cpt, int h[]) {

  FILE *fichOut ;
  int j ;

  
  fichOut = fopen(nomFichier, "w") ;
  if (!fichOut) {
    printf("erreur à l'ouverture du fichier %s\n", nomFichier) ;
    exit(2) ;
  }
  
  printf("écriture du fichier %s\n", nomFichier) ;
  fprintf(fichOut, "nombre de caractères lus : %ld\n", cpt) ;
  fclose(fichOut) ;
}

/********************
* 
* Calcul de la fréquence d'apparition des lettres dans un fichier texte.
* (génération de fichiers de progression)
*
******************/


int main(int argc, char* argv[]) 
{
  FILE *fichierCorpus ;
  char nomFichierGenere[100] ;
  int i, car, nbLignes, cptFichier,j ;
  long int nbCar ;
  char chaine[TAILLE] ;
  int nbLignesFichier=0;
  /* Tableau pour l'histogramme*/
  int histo[TAILLE_NB_LETTRE] = {0} ;
  int key;
  int semid;
    key=ftok(FILENAME,KEY);
   if(key==-1)
     perror(strcat(__FILE__," key=-1"));
    int shid=shmget(key,(TAILLE_NB_LETTRE)*sizeof(int),IPC_CREAT|0660);
    if(shid==-1)
      perror(strcat("shmget()",__FILE__));
 
      vGobal=(int *)shmat(shid,NULL,0);
     if(vGobal==NULL)
       perror(strcat("vGlobal=null",__FILE__));
     
  if (argc != 6) {
    printf("usage : %s <fichier à traiter> <nom générique fichiers résultats>\n", argv[0]) ;
    exit(1) ;
  }

  fichierCorpus = fopen(argv[1], "r") ;
  if (!fichierCorpus) {
    printf("erreur à l'ouverture du fichier %s\n", argv[1]) ;
    exit(2) ;
  }
 
 int skipline=atoi(argv[3]);//recuperer le nombre de ligne a sauter

 int sizeToSekk=0;//le nombre d'octet a sauter
  while (fgets(chaine, TAILLE, fichierCorpus)) {
         if(skipline>0)
	 {
	   skipline--;
	   sizeToSekk+=strlen(chaine);//calculer le nombre d'octet a sauter
	 }
        nbLignesFichier++;
  } 
 
  int noSkipline=atoi(argv[4]);//nombre de ligne a lire 
 
  fseek(fichierCorpus, sizeToSekk, SEEK_SET); //on a la position sizeToSekk au début du fichier
  nbCar = 0 ;
  nbLignes = 0 ;
  cptFichier = 0 ;
  noSkipline=(noSkipline==0?(nbLignesFichier):nbLignesFichier-skipline);
  /* Traitement ligne par ligne */
  while (fgets(chaine, TAILLE, fichierCorpus)&&nbLignes<=noSkipline) {
    
	/* Décompte des lettres (minuscules)*/
	for (i = 0 ; i < strlen(chaine) ; i++) {
		car = chaine[i] - 'a' ;
		if ((car >= 0) && (car < 26)) {
			histo[car] ++ ;
			nbCar++ ;
		}
	}
    
	/* Génération du fichier de décompte */
	nbLignes++ ;
	if (nbLignes % 10000 == 0) {
		sprintf(nomFichierGenere, "%s_%d_%d.txt", argv[2], getpid(), cptFichier) ;
		ecrireFichier(nomFichierGenere, nbCar, histo) ;	
		printf(".");fflush(stdout);
		cptFichier++ ;
	}
  }
  printf("\n");
  
  /* Ecriture dans le dernier fichier */
  sprintf(nomFichierGenere, "%s_%d_final.txt", argv[2], getpid()) ;
  ecrireFichier(nomFichierGenere, nbCar, histo) ;
  printf("Dernier fichier généré : %s [nombre de caractères traités : %ld, nb lignes traitées : %d sur %d]\n", nomFichierGenere, nbCar, nbLignes, nbLignesFichier) ;
  int numsem=0;
  int sem_id;
  if(sem_creation(&sem_id,1)==-1)
     {
     printf("A la ligne %d fichier %s ",__LINE__,__FILE__);
     perror("\t sem_creation()");
     }
  if((numsem=sem_recup(sem_id,1))==-1)
     {
     printf("A la ligne %d fichier %s ",__LINE__,__FILE__);
     perror("\tsem_recuv()");
     }
     
  for (j = 0 ; j < TAILLE_NB_LETTRE ; j++) {
     P(semid,numsem);
     *(vGobal+j)=histo[j];
      V(semid,numsem);
  }

  fclose(fichierCorpus) ;
  return(0) ;
}
