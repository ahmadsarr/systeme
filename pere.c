#include"pere.h"
#include"mes_semaphores.h"
#include<string.h>
int main(int argc, char* argv[])
{
  int key=ftok(FILENAME,KEY);
  int *vGobal;
   char *nomfichier=argv[1];
  char * nomgenerique=argv[2];
  int shid=shmget(key,(TAILLE_NB_LETTRE)*sizeof(int),IPC_CREAT|0666);
  //int proc=-1;
  int i;
  int total=0;
  int nbLignesFichier;
  int status;
  int sem_id;
  char str_deb[10];
  char str_fin[10];
 char str_nbproc[10];
 int nbproc;
 if (argc != 4) {
    printf("usage : %s <fichier à traiter> <nom générique fichiers résultats> <nombres de processus >\n", argv[0]) ;
    exit(1) ;
  }
  if(shid==-1)
    perror(strcat("shmget",__FILE__));
    
  vGobal=(int *)shmat(shid,NULL,0);
  if(vGobal==NULL)
     perror(strcat("shmat()",__FILE__));
  nbLignesFichier=nbLigne(argv[1]);
   nbproc=atoi(argv[3]);//recupere le nombre de processus
   int deb=0;
   int fin=nbLignesFichier/nbproc;
   sprintf(str_nbproc,"%d",nbproc);
   if(sem_creation(&sem_id,1)==-1)
     {
     printf("A la ligne %d fichier %s \n",__LINE__,__FILE__);
     perror("\t sem_creation()");
     }
    if(sem_initialisation(sem_id,0,1)==-1)//mutex
     {
     printf("A la ligne %d fichier %s \n",__LINE__,__FILE__);
     perror("\t");
     }
     int proc=-1;
  for(i=1;i<=nbproc;i++)
  {
     proc=fork();
     if(proc==0)//on est dans le fils
  {
     sprintf(str_deb,"%d",deb);
     sprintf(str_fin,"%d",fin);
   execl("./Lecteur","./Lecteur",nomfichier,nomgenerique,str_deb,str_fin,str_nbproc,NULL);
   printf("erreur execl");
   exit(-1);
  }else if(proc==-1){
   perror(strcat("creation fils",__FILE__));
   exit(-1);
   }else
   {
    deb+=fin+1;
    if((deb+fin)==nbLignesFichier-1)
       fin=0;
   }
  }
  
  while(nbproc>0){
   wait(&status);
   nbproc--;
   }
   double ratio=0;
   double verifPer=0;
   for(i=0;i<TAILLE_NB_LETTRE;i++)
   {
    total+=vGobal[i];
   }
   for(i=0;i<TAILLE_NB_LETTRE;i++)
   {
   if(vGobal[i]==0)
      continue;
     ratio=(vGobal[i]*100)/(double)total;
     verifPer+=ratio;
     printf(" Le caractere: %c frequence:%d  pourcentage : %.3f \n",'a'+i,vGobal[i],ratio);
     //ratio+=vGobal[i];
     
   }
     printf("%s nombre de carctere lu %.3f\n",__FILE__,verifPer);
   shmdt(vGobal);
   semctl(sem_id, IPC_RMID, 0);
  shmctl(shid,IPC_RMID,0);
}


///
int nbLigne(char *file)
{
  int nbLignesFichier=0;
  char chaine[TAILLE];
  FILE *fichierCorpus = fopen(file, "r") ;
  if (!fichierCorpus) {
    printf("erreur à l'ouverture du fichier %s\n", file) ;
    exit(2) ;
  }
  while (fgets(chaine, TAILLE, fichierCorpus)) {
        nbLignesFichier++;
  } 
  fclose(fichierCorpus);
  return nbLignesFichier;
}
