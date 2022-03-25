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
	


int sharedMemory()
{
     key_t          ShmKEY=123456; ;
     int            ShmID = 0;
     struct Memory  *ShmPTR;
     INT32 arr[4] = {4,3,2,1};
     int err=0;
     int sec_cnt = 0;

     //ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | IPC_EXCL | 0666); // remove flags if key already exisits 
     ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666); // remove flags if key already exisits
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
 //ShmPTR->status = NOT_READY;
     //ShmPTR->pkt_index = 1024;
     //ShmPTR->data_cnt = 4;
     ShmPTR->data  = 0;
     //ShmPTR->data[1]  = arr[1];
     //ShmPTR->data[2]  = arr[2];
     //ShmPTR->data[3]  = arr[3];
     printf("Server has filled %d to shared memory...\n",
            ShmPTR->data);
     //ShmPTR->status = FILLED;

     while (false)
     {
     	/* printf("Server has filled %d %d %d %d to shared memory...\n",
            ShmPTR->data[0], ShmPTR->data[1], 
            ShmPTR->data[2], ShmPTR->data[3]);
        printf("\r%d %d %d sleeping ...",sec_cnt,ShmPTR->status,ShmPTR->pkt_index);
        fflush(stdout);
        sec_cnt += 1;*/
        sleep(10);
     }

     printf("Data %d \n", ShmPTR->data);
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
 }

