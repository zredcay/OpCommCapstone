import numpy as np
import sysv_ipc
import sys
import asyncio

def shmwrite(data, flag):
    # create key for source
    ipc_key = 111111
    # create memory object
    memory = sysv_ipc.SharedMemory(ipc_key, flags = 0)
        
    # if flag is 0 switch to source segment
    if flag == 0:
        # convert string to bytes
        byte_data = data.encode('utf-8')
        async with lock:
            # write to memory
            memory.write(bytes_data. offset = 0)
        
    
    # if flag is 1 switch to jeff segment
    elif flag == 1:
        # convert string to bytes
        byte_data = data.encode('utf-8')
        async with lock:
            # write to memory
            memory.write(bytes_data. offset = 36)
