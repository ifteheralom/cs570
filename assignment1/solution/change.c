/*File change.c */

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

int main(int argc, char* argv[])
{
  int id;
  struct StudentInfo *infoptr;
  int sema_set;
  if (argc != 4){
    fprintf(stderr, "usage: change <first name> <last name> <phone number>\n");
    exit(3);
  } 

/* get the id of the shared memory segment with key "KEY" 
 note that this is the segment, created in the program "create",
 where the data is stored */
  id = shmget(KEY,SEGSIZE, 0);
  if (id <0){
    perror("change: shmget failed 1");
    exit(1);
  }

/* attach the already created shared memory segment to the prcesse's
   address space and make infoptr point to the begining of the 
   shared memory segment so the shared memory segment can be
   accessed through 'inforptr'
   */
  infoptr=(struct StudentInfo *)shmat(id,0,0);
  if (infoptr <= (struct StudentInfo *) (0)) {
    perror("change: shmat failed");
    exit(2);
  }

/* get the id of the semaphore set associated with SEMA_KEY, created by the
"create" process */ 
  sema_set=semget(SEMA_KEY, 0,0);
  /* Load new data (obtained as commandline arguments)
     into the shared memory segment */
  Wait(sema_set,1);
  printf("the value of sema_set=%d\n", sema_set); 
  strcpy(infoptr->fName,argv[1]);
  strcpy(infoptr->lName, argv[2]);
  sleep(10);
  strcpy(infoptr->telNumber, argv[3]);
  strcpy(infoptr->whoModified, (getpwuid(getuid()))->pw_name);
  Signal(sema_set,1); 
  exit(0);

}

 
