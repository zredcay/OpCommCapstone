#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include "sharedMemory.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

struct shared createMemory()
{
     key_t          ShmKEY = 111111;
     int            ShmID = 0;
     struct Memory  *ShmPTR;
     float arr[18];
     int err=0;
     struct shared ex;



     ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | IPC_EXCL | 0666); // remove flags if key already exisits
     //ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666); // remove flags if key already exisits

     if (ShmID < 0)
     {
        // for case of error does not return a valid shmid
        err = errno;
        printf("Error getting shared memory id %d %d\n",ShmID,err);
        if(err == EEXIST) printf("memory exist for this key\n");
        exit(1);
     }

     ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
     if ((int) ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     ///****************Fills memory with numbers for debugging*************

     for (INT32 i = 0; i < 18; i++) {
      	ShmPTR->data[i] = 0;
      	//printf("array %i is %f\n",i,ShmPTR->data[i]);
     }
     ex.ShmKEY = ShmKEY;
     ex.ShmID = ShmID;
     ex.ShmPTR = ShmPTR;
     return ex;

}

sem_t* createNamedSem()
{

     char* SEM_NAME = "opcapstone";
     sem_t* mutex;



    if((mutex = sem_open(SEM_NAME, O_CREAT, 0600, 1)) == SEM_FAILED) {
    	perror("sem_open");
    	sem_unlink(SEM_NAME);
    	exit(EXIT_FAILURE);
    }

	return mutex;

}

int closeNamedSem(sem_t* mutex)
{

    char* SEM_NAME = "opcapstone";
    sem_close(mutex);
    sem_unlink(SEM_NAME);
    return 0;

}

int closeMemeory(struct shared ex)
{
     key_t ShmKEY = ex.ShmKEY;
     int ShmID = ex.ShmID;
     struct Memory *ShmPTR = ex.ShmPTR;


     shmdt((void *) ShmPTR);
     printf("DETACHING SHARED MEMORY\n");
     shmctl(ShmID, IPC_RMID, NULL);

     printf("REMOVING SHARED MEMORY\n");

}

struct Memory sharedMemory(int flag, struct shared ex, sem_t* mutex)
{
     key_t ShmKEY = ex.ShmKEY;
     int ShmID = ex.ShmID;
     struct Memory *ShmPTR = ex.ShmPTR;
     struct Memory arr;
     int err=0;
     int offset = 0;
     int max = 18;

     sem_wait(mutex);////**************8waiting till semaphore is open to read from **************

     //printf("flag is %i\n", flag);

     if(flag == 1)
     {
     		offset = 9;
     		max = 	18;
     }


     for(int i = offset; i < max; i++)
     {
     	  arr.data[i] = ShmPTR->data[i];
     	  //printf("Server has filled %f to shared memory...\n", arr.data[i]);
     }

     memset(ShmPTR->data, NULL, 18);

     sem_post(mutex); // *********closing access to sempahore**********

	return arr;

 }

