#include "def.h"

/* Fonction d'écriture de la répartition dans un fichier */
void ecrireFichier(char *nomFichier, long int cpt, int r[]) {

  FILE *fichOut ;
  int j ;
   
  fichOut = fopen(nomFichier, "w") ;
  if (!fichOut) {
    printf("erreur à l'ouverture du fichier %s\n", nomFichier) ;
    exit(2) ;
  }
  
  //printf("Ecriture du fichier %s\n", nomFichier) ;
  fprintf(fichOut, "nombre de mots lus : %ld\n", cpt) ;
  for (j = 0 ; j < TAILLE_NB_MOT ; j++) {
    fprintf(fichOut, "%d : %d\n", j, r[j]) ;
  }
  fclose(fichOut) ;
}

/********************
* 
* Calcul de la répartition du nombre de mot dans un phrase dans un fichier texte.
* (génération de fichiers de progression)
*
******************/


int main(int argc, char* argv[]) 
{
  FILE *fichierCorpus ;
  char nomFichierGenere[100] ;
  int i, j, compteurLignes, nbCar, nbMot, nbLignes, cptFichier ;
  long int nbMotTotal ;
  char chaine[TAILLE] ;
  int nbLignesFichier=0;

  /* Tableau pour la repartition */
  int repartition[TAILLE_NB_MOT] = {0} ;

  /* recuperation de cle pour la recuperation du tableau partage*/
  key_t clef = ftok(PATH, ID);
  int id = shmget(clef, TAILLE_NB_MOT*sizeof(int), 0);
 
  if(id == -1){
    perror("LecteurCorpus: erreur dans la creation du segment de memoire partagee\n");
    exit(1);
  }

  int* p_tableau_partage = (int*) shmat(id, NULL, 0);
  if(p_tableau_partage == NULL)
    {
      perror("LecteurCorpus: erreur dans l'attachement du pointeur sur le tableau partagé\n");
      exit(1);
    }
  
  /* recuperation du semaphore */
  int sem_id;
  if(sem_recup(&sem_id,1) == -1){
    perror("LectureCorpus : erreur dans la recuperation du semaphore\n"); 
  }
  
  if (argc != 5) {
    printf("usage : %s <fichier à traiter> <nom générique fichiers résultats> <num de la ligne pour debuter la lecture> <nb de lignes a traiter>\n", argv[0]) ;
    exit(1) ;
  }

  fichierCorpus = fopen(argv[1], "r") ;
  if (!fichierCorpus) {
    printf("LecteurCorpus: erreur à l'ouverture du fichier %s\n", argv[1]) ;
    exit(2) ;
  }

  /* calcul du nombre de lignes total du fichier */ 
  while (fgets(chaine, TAILLE, fichierCorpus)) {
    nbLignesFichier++;
  } 
 
  fseek(fichierCorpus, 0, SEEK_SET); //on se replace au début du fichier
  
  nbCar = 0 ;
  nbMot = 0 ;
  nbMotTotal = 0;
  nbLignes = 0 ;
  cptFichier = 0 ;
  compteurLignes = atoi(argv[3]);
  j = atoi(argv[4]);

  /* On saute les ligne que l'on ne va pas lire */
  for(i = 0 ; i < compteurLignes ; i++){
    fgets(chaine, TAILLE, fichierCorpus);
  }
  /* Traitement ligne par ligne */
  while (fgets(chaine, TAILLE, fichierCorpus) && nbLignes < j) {
    

    /* Décompte des mots */
    for (i = 0 ; i < strlen(chaine) ; i++) {
      //si c'est un caractère de fin de mot, on incrémente (sauf si successif).
      if(strchr(" ,;:.!?\n",chaine[i])!=NULL){
	if(nbCar!=0 ){
	  nbCar=0;
	  nbMot++;
	}
      }
      else 
	nbCar++;
    }
    //dernier mot si la ponctuation est manquante
    if(nbCar!=0)
      nbMot++;
    //printf("ligne %d : %d mots\n", nbLignes, nbMot); //pour debug
    if(nbMot<=TAILLE_NB_MOT-1)
      repartition[nbMot] ++;
    else	
      repartition[TAILLE_NB_MOT-1]++;
    nbMotTotal+=nbMot;
    nbCar=0;
    nbMot=0;
	
    /* Génération du fichier de décompte */
    nbLignes++ ;
    if (nbLignes % 10000 == 0) {
      sprintf(nomFichierGenere, "%s_%d_%d.txt", argv[2], getpid(), cptFichier) ;
      ecrireFichier(nomFichierGenere, nbMotTotal, repartition) ;	
      printf(".");fflush(stdout);
      cptFichier++ ;
    }
  }
  printf("\n");
  
  for(i = 0 ; i < TAILLETABLEAU ; i++){
    P(sem_id, 0);
    p_tableau_partage[i] += repartition[i];
    V(sem_id, 0);
  }
  
  /* Ecriture dans le dernier fichier */
  sprintf(nomFichierGenere, "%s_%d_final.txt", argv[2], getpid()) ;
  ecrireFichier(nomFichierGenere, nbMotTotal, repartition) ;
  printf("[%d] Dernier fichier généré : %s [nombre de mot traités : %ld, nb lignes traitées : %d sur %d]\n", getpid(),nomFichierGenere, nbMotTotal, nbLignes, nbLignesFichier) ;

   
  fclose(fichierCorpus) ;
  return(0) ;
}
