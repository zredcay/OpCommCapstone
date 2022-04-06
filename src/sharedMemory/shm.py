import numpy as np
import sysv_ipc
import posix_ipc
import sys
from ctypes import c_float

def shmwrite(data, flag):
    # old way of doing things
    # create key for shm
    ipc_key = 111111
    
    # create name for semaphore
    sem_name = 'opcapstone'
    
    # create memory object
    memory = sysv_ipc.SharedMemory(ipc_key, flags = 0)
    
    # split string into separate values
    #ax, ay, az, gx, gy, gz, mx, my, mz = data.split()
    
    # convert from string to c float
    #ax = c_float(float(ax))
    #ay = c_float(float(ay))
    #az = c_float(float(az))
    #gx = c_float(float(gx))
    #gy = c_float(float(gy))
    #gz = c_float(float(gz))
    #mx = c_float(float(mx))
    #my = c_float(float(my))
    #mz = c_float(float(mz))
        
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