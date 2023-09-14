#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pwd.h>
#include "header.h"

int main(int argc, char* argv[]) {
    int id, id2;
    int sema_set;

    if (argc != 2 || strlen(argv[1]) != 9){
        fprintf(stderr, "usage: query <studentID>\n");
        exit(3);
    } 

    struct StudentInfo *infoptr = NULL;
    struct StudentInfo *infoptrStart = NULL;
    struct ReadCounter *readPointer = NULL;

    /* get shared memory to store data*/
    id = shmget(KEY, SEG_STD_INFO_SIZE, IPC_CREAT|0666);
    id2 = shmget(KEY, SEG_READ_SIZE, IPC_CREAT|0666);
    if (id < 0 || id2 < 0) {
        perror("create: shmget failed");
        exit(1);
    }

    /*attach the shared memory segment to the process's address space */
    infoptrStart = (struct StudentInfo *)shmat(id ,0, 0); 
    readPointer = (struct ReadCounter *)shmat(id2, 0, 0);
    if (infoptrStart <= (struct StudentInfo *) (0) || readPointer <= (struct ReadCounter *) (0)) {
        perror("create: shmat failed");
        exit(2);
    }

    sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS); /* get a set of NUM_SEMAPHS semaphores*/
    if ((sema_set < 0) ){
        perror("create: semget failed");
        exit(2);
    }

    printf("\n\n-----***************-------\n-----SHM QUERY  BEGIN------\n-----***************------\n\n");
    infoptr = infoptrStart + 1;
    bool matchFound = false;
    for(int i=0; i<=50; i++) {
        Wait(sema_set, 1); 
        readPointer->readCount = readPointer->readCount + 1;

        /*
        printf(
            "Matching: %s %s \n",
            argv[1], infoptr->StudentId 
        );
        */

        if (strncmp(argv[1], infoptr->StudentId, 9) == 0) {
            matchFound = true;
            printf("%d: %s%s%s%s \n", i, 
                infoptr->Name,
                infoptr->StudentId,
                infoptr->Address,
                infoptr->telephoneNumber
            );
            sleep(0);
            Signal(sema_set,1);
            break;
        }       
        sleep(0);
        Signal(sema_set,1);
        infoptr++;
    }
    if (!matchFound) {
        printf("\n ***No matching records found!");
        Signal(sema_set,1);
    }
    infoptr = infoptrStart + 1;

    printf("\n\n-----***************-------\n-----SHM  QUERY   END------\n-----***************-------\n\n");

    shmdt((char  *)infoptr); /* detach the shared memory segment */
    shmdt((char  *)readPointer); /* detach the shared memory segment */
    
    shmctl(id, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segment*/
    shmctl(id2, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segment*/
    
    semctl(sema_set, 0, IPC_RMID); /*Remove the semaphore set */
    exit(0);
}