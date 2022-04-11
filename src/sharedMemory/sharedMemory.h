#ifndef _sharedMemory_h_
#define _sharedMemory_h_

#include <semaphore.h>



struct shared createMemory();
sem_t* createNamedSem();
struct Memory sharedMemory(int flag, struct shared, sem_t* mutex);
int closeNamedSem(sem_t* mutex);
int closeMemeory(struct shared);

typedef signed int INT32;
#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

struct Memory
{
     float data[18];

};
 struct shared
 {
     key_t          ShmKEY;
     int            ShmID;
     struct Memory  *ShmPTR;
 };

#endif
