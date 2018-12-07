#ifndef MES_SEMAPHORES_H
#define MES_SEMAPHORES_H
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#define FILENAME_SEM "./mes_semaphores.c"
#define KEY_SEM "2"
typedef union semun {
int val ;
struct semid_ds *buf ; 
unsigned short *array ;
struct seminfo *__buf; 
} semval;
int sem_creation(int * semid,int nbsemaphore);
int sem_initialisation(int semid,int num_semaphore,int nbr_jetons);
int P(int semid,int num_semaphore);
int V(int semid,int num_semaphore);
int sem_destruction(int semid);
int sem_recup(int *semid,int nbsemaphore);
#endif