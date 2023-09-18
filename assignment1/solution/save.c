#include <stdlib.h>
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

int main() {
    int id, id2;
    int sema_set;

    FILE* ptr = fopen("sample_input_file_save.txt", "w+");

    struct StudentInfo *infoptr = NULL;
    struct StudentInfo *infoptrStart = NULL;
    struct ReadCounter *readPointer = NULL;
 
    if (ptr == NULL) {
        printf("file can't be opened \n");
    }

    /* get shared memory to scleartore data*/
    id = shmget(KEY, SEG_STD_INFO_SIZE, 0);
    id2 = shmget(KEY, SEG_READ_SIZE, 0);
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

    printf("\n\n-----***************------\n-----FILE SAVE BEGIN------\n-----***************------\n\n");
    infoptr = infoptrStart + 1;
    for(int i=0; i<=50; i++) {
        Wait(sema_set, 1); 
        readPointer->readCount = readPointer->readCount + 1;

        if(strcmp("", infoptr->StudentId) != 0){
            fprintf(ptr, "%s%s%s%s",
                infoptr->Name,
                infoptr->StudentId,
                infoptr->Address,
                infoptr->telephoneNumber
            );

            printf("%d: %s%s%s%s \n", i, 
                infoptr->Name,
                infoptr->StudentId,
                infoptr->Address,
                infoptr->telephoneNumber
            );
        }        

        sleep(0);
        Signal(sema_set, 1);
        infoptr++; 
    }
    Signal(sema_set,1); 
    infoptr = infoptrStart + 1;
    printf("\n\n-----***************------\n-----FILE  SAVE  END------\n-----***************------\n\n");

    fclose(ptr);
    
    shmdt((char  *)infoptrStart); /* detach the shared memory segment */
    shmdt((char  *)readPointer); /* detach the shared memory segment */
    
    shmctl(id, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segment*/
    shmctl(id2, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segment*/
    
    semctl(sema_set, 0, IPC_RMID); /*Remove the semaphore set */
    exit(0);
}