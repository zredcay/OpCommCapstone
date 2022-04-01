import numpy as np
import sysv_ipc
import posix_ipc
import sys

def shmwrite(data, flag):
    # old way of doing things
    # create key for shm
    ipc_key = 111111
    
    # create name for semaphore
    sem_name = 'opcapstone'
    
    # create memory object
    memory = sysv_ipc.SharedMemory(ipc_key, flags = 0)
        
    try:
        # create sem object using the name created in main.c
        sem = posix_ipc.Semaphore(sem_name, 0, 0o600, 1)
        
        # acquire access to the semaphore with a timeout of 1 s
        sem.acquire(1)
        # if flag is 0 switch to source segment
        if flag == 0:
            # convert string to bytes
            bytes_data = data.encode('utf-8')
            # write to memory
            memory.write(bytes_data, offset = 0)
            

        # if flag is 1 switch to jeff segment
        elif flag == 1:
            # convert string to bytes
            bytes_data = data.encode('utf-8')
            # write to memory
            memory.write(bytes_data, offset = 36)
    finally:
        sem.release()
