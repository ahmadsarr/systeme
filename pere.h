#ifndef PERE_H
#define PERE_H
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>

#define TAILLE 1024
#define TAILLE_NB_LETTRE 26
#define FILENAME "./pere.h"
#define KEY 2

int nbLigne(char *file);
#endif