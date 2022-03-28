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

struct shared createMemory()
{
     key_t          ShmKEY = 111111; 
     int            ShmID = 0;
     struct Memory  *ShmPTR;
     INT32 arr[72];
     int err=0;
     struct shared ex;
     
     for (INT32 i = 0; i < 72; i++) {
    	arr[i] = rand()%10;
    	printf("array %i is %i\n",i,arr[i]);
     }			
     
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
     for (INT32 i = 0; i < 72; i++) {
      	ShmPTR->data[i] = arr[i];
      	printf("array %i is %i\n",i,ShmPTR->data[i]);
     }
     ex.ShmKEY = ShmKEY;
     ex.ShmID = ShmID;
     ex.ShmPTR = ShmPTR;
     return ex;
		
}

int closeMemeory(struct shared ex)
{ 
     key_t ShmKEY = ex.ShmKEY;
     int ShmID = ex.ShmID;
     struct Memory *ShmPTR = ex.ShmPTR; 
     
     
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);

     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);

}

int sharedMemory(int flag, struct shared ex)
{
     key_t ShmKEY = ex.ShmKEY;
     int ShmID = ex.ShmID;
     struct Memory *ShmPTR = ex.ShmPTR; 
     INT32 arr[72];
     int err=0;
     int offset = 0;
     int max = 36;
     
     
  	
     sleep(10);
     pthread_mutex_t  init_lock =  PTHREAD_MUTEX_INITIALIZER;

     printf("flag is %i\n", flag);	
	
     if(flag == 1)
     {
     		offset = 36;
     		max = 72;
     }
     
     pthread_mutex_lock(&(init_lock));    
     for(int i = offset; i < max; i++)
     {
     	  arr[i] = ShmPTR->data[i];
     	  printf("Server has filled %d to shared memory...\n", arr[i]);
     	   
     	    
     	  
     }

     memset(ShmPTR->data, NULL, 36);
  
  for(int i = offset; i < max; i++)
     {
     	  
     	  printf("Server has filled %d to shared memory...\n", arr[i]);
     	   
     	    
     	  
     }
    pthread_mutex_unlock(&(init_lock));
	return 0;
 }

