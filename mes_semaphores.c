#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


typedef union semun {
  int val; //valeur pour SETVAL
  struct semid_ds *buf; //buffer pour IPC_SET et IPCSTAT
  unsigned short *array; // tableau pour GETALL et SETALL
  struct seminfo *__buff; // buffe pour IPC_INFO 
} semval;


/*----- PROTOTYPES ----*/
int sem_creation(int* semid, int nombre_semaphores);

int sem_initialisation(int semid, int num_semaphore, int nbr_jetons);

int P(int semid, int num_semaphore);

int V(int semid, int num_semaphore);

int sem_destruction(int semid);

int sem_recup(int *semid, int nb_semphores);


/*----- FONCTIONS ----*/
int sem_creation(int* semid, int nombre_semaphores){
  key_t cle = ftok("./mes_semaphores.c", 2);
  if(cle == -1)
    {
      perror("erreur dans la creation de la cle pour les semaphors\n");
      return -1;
    }
 
  *semid = semget(cle, nombre_semaphores, IPC_CREAT|0660);
  if(*semid == -1)
    {
      perror("Erreur dans la creation des semaphores\n");
      return -1;
    }
  return 0;
}

int sem_initialisation(int semid, int num_semaphore, int nbr_jetons){
  semval seminit;
  seminit.val = nbr_jetons;
  return semctl(semid, num_semaphore, SETVAL, seminit);
}

int P(int semid, int num_semaphore){
  struct sembuf smbf;
  smbf.sem_num = num_semaphore;
  smbf.sem_op = -1;
  smbf.sem_flg = 0;
  return semop(semid, &smbf, 1);
}

int V(int semid, int num_semaphore){
  struct sembuf smbf;
  smbf.sem_num = num_semaphore;
  smbf.sem_op = 1;
  smbf.sem_flg = 0;
  return semop(semid, &smbf, 1);
}

int sem_destruction(int semid){
  semctl(semid, IPC_RMID, 0);
  return 0;
}

int sem_recup(int *semid, int nb_semaphores){
  if(nb_semaphores < 0){
    return 0;
  }
  key_t cle = ftok("./mes_semaphores.c", 1);
  if(cle == -1)
    {
      perror("erreur dans la creation de la cle pour les semaphors\n");
      return -1;
    }
    
  *semid = semget(cle, nb_semaphores, 0);
  if(*semid == -1)
    {
      return -1;
    }
  return 0;
}

//semctl(id, SETVAL, varstruct)  id = resultat semget()   varstruct = variable de type structue semval (vue en cours) 

/* semaphore d'exclusion mutuelle => init a 1 (mutex)
   semaphore pour protege 1 ensemble de ressources => init a nbDeJeton
   sempaphore de synchronisation => init a 0
*/
