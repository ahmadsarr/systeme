#include <signal.h>
#include <sys/wait.h>
#include "def.h"



int main(int argc, char** argv){
  if(argc != 4){
    perror("les arguments a passer sont : <le nom du fichier a traiter> <le nom du fichier de sortie> <nb processus a creer pour lire>\n");
    exit(1);
  }

  int status, i, nb_de_fork;
  int compteur = 0;
  char chaine[TAILLE] ;
  FILE *fichierCorpus;

  nb_de_fork = atoi(argv[3]);

  fichierCorpus = fopen(argv[1], "r") ;
  if (!fichierCorpus) {
    printf("Pere: erreur à l'ouverture du fichier %s\n", argv[1]) ;
    exit(2) ;
  }
  /* compteur lignes*/
  while (fgets(chaine, TAILLE, fichierCorpus)) {
    compteur++;
  } 
  fclose(fichierCorpus);
  
  int sem_id;
  if(sem_creation(&sem_id, nb_de_fork) == -1){
    perror("erreur lors de la creation sur semaphore\n");
  }
  for(i = 0 ; i < nb_de_fork ; i++){
    if(sem_initialisation(sem_id, i, 1) == -1){
      perror("erreur lors de l\'initialisation du semaphore\n");
    }
  }
  

  key_t cle = ftok(PATH, ID); //cle pour le segment de memoire partagee
  int mem_id = shmget(cle, TAILLETABLEAU*sizeof(int), IPC_CREAT|0644);

  if(mem_id == -1){
    perror("Pere: erreur dans la creation du segment de memoire partagee\n");
    exit(1);
  }
  
  int *pTableauPartage;
  pTableauPartage = (int*) shmat(mem_id, NULL, 0); //shmat(id, adresse, option)
  if(pTableauPartage == NULL){
    perror("Pere: erreur lors de l attachement du segment de memoire partagee\n");
    exit(1);
  }
  

  pid_t ret[nb_de_fork];
  int ligneDeb, nb_de_lignes;
  char lDebut[10];
  char nbLignes[10];
  
  nb_de_lignes = (compteur/nb_de_fork)-1;
  sprintf(nbLignes, "%d", nb_de_lignes);
  for(i = 0 ; i < nb_de_fork ; i++)
    {
  printf("num du fork %d\n",i);

      /* palie le probleme nombre de ligne totale impaire*/
      if(i+1 == nb_de_fork && compteur%(compteur/nb_de_fork) != 0){
	nb_de_lignes += compteur%(compteur/nb_de_fork);
	sprintf(nbLignes, "%d", nb_de_lignes);
      }
      
      ligneDeb = i * (compteur/nb_de_fork);
      sprintf(lDebut, "%d", ligneDeb); // transforme le int en string

      switch(ret[i] = fork())
	{
	case (pid_t)-1: //erreur d'execution
	  perror("le fork n'a pas fonctionne!\n\n");
	  exit(1);
	case (pid_t)0: //On se trouve dans le fils
	  execl("./LectureCorpus","LectureCorpus", argv[1] , argv[2], lDebut, nbLignes, NULL);
	  break;
	default: //On est dans le pere
	  break;
	}
    }
  i = 0;
  while(i < nb_de_fork){
    waitpid(ret[i], &status, 0);   //ret contient le pid du fils
    i++;
  }
  /* statistiques du nb de lignes traitees */
  compteur = 0;
  for(i = 0 ; i < TAILLETABLEAU ; i++){
    compteur += pTableauPartage[i];
  }
  for(i = 0 ; i < TAILLETABLEAU ; i++){
    printf("frequence d\'apparition d\'une phrase de %d mots dans le fichier : %d/%d\n",i,pTableauPartage[i],compteur);
  }
  shmdt(pTableauPartage); // detachement
  shmctl(mem_id, IPC_RMID, 0); // destruction
  sem_destruction(sem_id);
  printf("le pere et le fils ont termines\n");
  return 0;
}
