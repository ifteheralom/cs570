
/* header.h */
#define KEY  ((key_t)(124345)) /*change it to last five digits of your SID*/
#define SEGSIZE  sizeof(struct StudentInfo)

#define NUM_SEMAPHS 5
#define SEMA_KEY   ((key_t)(44456)) /* change this to last four digits of SID */

struct StudentInfo{
  char Name[20];
  long StudentID;
  char Address[50];
  long TelephoneNumber;
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);