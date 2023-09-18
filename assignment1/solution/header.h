
/* header.h */
#define KEY  ((key_t)(12970)) /*change it to last five digits of your SID*/
#define SEGSIZE sizeof(struct StudentInfo)
#define SEG_STD_INFO_SIZE 55*(sizeof(struct StudentInfo))
#define SEG_READ_SIZE sizeof(struct ReadCounter)

#define NUM_SEMAPHS 5
#define SEMA_KEY   ((key_t)(44450)) /* change this to last four digits of SID */

struct StudentInfo {
  char Name[80];
  char StudentId[80];
  char Address[80];
  char telephoneNumber[80];
  
  char fName[20];
  char lName[20];
  char telNumber[15];
  char whoModified[10];
};

struct ReadCounter {
  int readCount;
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);
