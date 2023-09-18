/*File change.c */

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

#define MODIFY_NAME 1
#define MODIFY_ADDRESS 2
#define MODIFY_TELEPHONE 3

bool modify_info(char *stdId, int attributeNumber, char* change, struct StudentInfo *infoptr) {
  bool matchFound = false;
  for(int i=0; i<=50; i++) {
      /*
      printf(
          "Matching: %s %s \n",
          stdId, infoptr->StudentId 
      );
      */
      if (strncmp(stdId, infoptr->StudentId, 9) == 0) {
          matchFound = true;
          strcat(change, "\n");

          switch (attributeNumber) {
            case MODIFY_NAME:
              strcpy(infoptr->Name, change);
              break;
            case MODIFY_ADDRESS:
              strcpy(infoptr->Address, change);
              break;
            case MODIFY_TELEPHONE:
              strcpy(infoptr->telephoneNumber, change);
              break;
            default:
              printf("Invalid attribute choice\n");
              break;
          }

          printf("%d: %s%s%s%s \n", i, 
              infoptr->Name,
              infoptr->StudentId,
              infoptr->Address,
              infoptr->telephoneNumber
          );
          break;
      }
      infoptr++;
  }
  return matchFound;
}

bool add_info(char *stdId, char *name, char *address, char *telephone, struct StudentInfo *infoptr) {
  bool matchFound = false;
  
  printf(
      "Adding: %s %s %s %s\n",
      stdId, name, address, telephone
  );

  //strcpy(stdId[strlen(stdId)], "\n");
  // strcat(name, "\n");
  // strcat(address, "\n");
  // strcat(telephone, "\n");

  for(int i=0; i<=50; i++) {
      /*printf(
          "Adding: %s %s %s %s\n",
          stdId, name, address, telephone 
      );*/
      
      if(strcmp("", infoptr->StudentId) == 0) {
        matchFound = true;

        char buf[80] = {};
        strncpy(buf, name, strlen(name));
        buf[strlen(name)] = '\n';
        strcpy(infoptr->Name, buf);
        
        char buf2[80] = {};
        strncpy(buf2, stdId, strlen(stdId));
        buf2[strlen(stdId)] = '\n';
        strcpy(infoptr->StudentId, buf2);
        
        char buf3[80] = {};
        strncpy(buf3, address, strlen(address));
        buf3[strlen(address)] = '\n';
        strcpy(infoptr->Address, buf3);

        char buf4[80] = {};
        strncpy(buf4, telephone, strlen(telephone));
        buf4[strlen(telephone)] = '\n';
        strcpy(infoptr->telephoneNumber, buf4);

        printf("%d: %s%s%s%s \n", i, 
            infoptr->Name,
            infoptr->StudentId,
            infoptr->Address,
            infoptr->telephoneNumber
        );
        break;
      }  
      infoptr++;
  }
  return matchFound;
}

bool delete_info(char *stdId, struct StudentInfo *infoptr) {
  bool matchFound = false;
  for(int i=0; i<=50; i++) {
      /*
      printf(
          "Matching: %s %s \n",
          stdId, infoptr->StudentId 
      );
      */
      if (strncmp(stdId, infoptr->StudentId, 9) == 0) {
          matchFound = true;

          printf("%d: %s%s%s%s \n", i, 
              infoptr->Name,
              infoptr->StudentId,
              infoptr->Address,
              infoptr->telephoneNumber
          );

          strcpy(infoptr->Name, "");
          strcpy(infoptr->StudentId, "");
          strcpy(infoptr->Address, "");
          strcpy(infoptr->telephoneNumber, "");
          break;
      }
      infoptr++;
  }
  return matchFound;
}

int main(int argc, char* argv[])
{
  int COMMAND;
  int id;
  struct StudentInfo *infoptr = NULL;
  struct StudentInfo *infoptrStart = NULL;
  int sema_set;
  if (argc < 4){
    fprintf(stderr, "usage: change <COMMAND> <student ID> <Attribute Number> <Change>\n");
    exit(3);
  } 

  COMMAND = atoi(argv[1]);

  if(strncmp(argv[2], "ABCD", 4) != 0) {
    printf("\nInvalid Password!, You are not authorized for this operation\n");
    exit(0);
  }
 
  /* get the id of the shared memory segment with key "KEY" 
 note that this is the segment, created in the program "create",
 where the data is stored */
  id = shmget(KEY, SEGSIZE, 0);
  if (id <0){
    perror("change: shmget failed 1");
    exit(1);
  }

  /*attach the already created shared memory segment to the prcesse's
    address space and make infoptr point to the begining of the 
    shared memory segment so the shared memory segment can be
    accessed through 'inforptr'
   */
  infoptrStart = (struct StudentInfo *)shmat(id,0,0);
  if (infoptrStart <= (struct StudentInfo *) (0)) {
    perror("change: shmat failed");
    exit(2);
  }
  infoptr = infoptrStart + 1;

  /* get the id of the semaphore set associated with SEMA_KEY, created by the
    "create" process */ 
  sema_set=semget(SEMA_KEY, 0,0);
  /* Load new data (obtained as commandline arguments)
    into the shared memory segment */
  

  if (COMMAND == 1) {
    printf("\n\n%s %s %s \n", argv[3], argv[4], argv[5]);
    printf("Record updating...\n\n");
    Wait(sema_set,1);
    bool result = modify_info(argv[3], atoi(argv[4]), argv[5], infoptr);
    infoptr = infoptrStart + 1;
    sleep(5);
    Signal(sema_set,1); 

    if (result) {
      printf("Record update Success !!\n");
    } else {
      printf("Record update FAILED !!\n");
    }
  }
  else if (COMMAND == 2) {
    printf("\n\n%s\n", argv[3]);
    printf("Record deleting...\n\n");
    Wait(sema_set,1);
    bool result = delete_info(argv[3], infoptr);
    infoptr = infoptrStart + 1;
    sleep(5);
    Signal(sema_set,1); 

    if (result) {
      printf("Record delete Success !!\n");
    } else {
      printf("Record delete FAILED !!\n");
    }
  }
  else if (COMMAND == 3) {
    printf("\n\n%s %s %s %s\n", argv[3], argv[4], argv[5], argv[6]);
    printf("Record Adding...\n\n");
    Wait(sema_set,1);
    bool result = add_info(argv[3], argv[4], argv[5], argv[6], infoptr);
    infoptr = infoptrStart + 1;
    sleep(5);
    Signal(sema_set,1); 

    if (result) {
      printf("Record add Success !!\n");
    } else {
      printf("Record add FAILED !!\n");
    }
  }

  // Wait(sema_set,1);
  
  // strcpy(infoptr->Name,argv[1]);

  // Signal(sema_set,1); 

  //shmdt((char  *)infoptrStart); /* detach the shared memory segment */
  //shmdt((char  *)readPointer); /* detach the shared memory segment */
  
  //shmctl(id, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segment*/
  //shmctl(id2, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segment*/
  
  //semctl(sema_set, 0, IPC_RMID); /*Remove the semaphore set */
  exit(0);
}
