import numpy as np
import sysv_ipc
import posix_ipc
import sys
import struct
def shmwrite(data, flag):
    # old way of doing things
    # create key for shm
    ipc_key = 111111
    
    # create name for semaphore
    sem_name = 'opcapstone'
    
    # create memory object
    memory = sysv_ipc.SharedMemory(ipc_key, flags = 0)
    
    # split string into separate values
    ax, ay, az, gx, gy, gz, mx, my, mz = data.split()
    
    # convert from string to c float
    ax = float(ax)
    ay = float(ay)
    az = float(az)
    gx = float(gx)
    gy = float(gy)
    gz = float(gz)
    mx = float(mx)
    my = float(my)
    mz = float(mz)
    
    try:
        # create sem object using the name created in main.c
        sem = posix_ipc.Semaphore(sem_name, 0, 0o600, 1)

        # acquire access to the semaphore with a timeout of 1 s
        sem.acquire(1)
        # if flag is 0 switch to source segment
        if flag == 0:
            # convert string to bytes
            bytes_data = struct.pack('fffffffff', ax, ay, az, gx, gy, gz, mx, my, mz)
            #bytes_data = struct.pack('h', 1, 2, 3, 4, 5, 6, 7, 8, 9)
            # write to memory
            memory.write(bytes_data, offset = 0)
            

        # if flag is 1 switch to jeff segment
        elif flag == 1:
            # convert string to bytes
            bytes_data = struct.pack('fffffffff', ax, ay, az, gx, gy, gz, mx, my, mz)
            #bytes_data = struct.pack('h', 1, 2, 3, 4, 5, 6, 7, 8, 9)
            # write to memory
            memory.write(bytes_data, offset = 36)
    finally:
        sem.release()