import numpy as np
import sysv_ipc
import sys

# Create shared memory object
ipc_key   = 123456
NOT_READY = -1
FILLED    = 0
TAKEN     = 1

PY_MAJOR_VERSION = sys.version_info[0]

def write_to_memory(memory, status, offset=0):
    #print("writing %s " % s)
    s = str(status) + '\0'
    if PY_MAJOR_VERSION > 2:
        s = s.encode()
    memory.write(s,offset=ofset)
    return;

try:
    memory = sysv_ipc.SharedMemory(ipc_key,flags=0)
    int_cnt = int(memory.size/4);
    print('int_cnt: ',int_cnt)
    if(int_cnt>3):
        # Read value from shared memory, byte_count=0 means all bytes
        #status = int.from_bytes(memory.read(byte_count=0,offset=0))
        #print('status:',status)
        c = np.ndarray((int_cnt,0), dtype=np.int)
        #memory_value = memory.read(byte_count=4,offset=0)
        
        for x in range(7):
                memory_value = memory.read(byte_count=4,offset=(x*4))
                c[x] = int.from_bytes(memory_value, "little", signed=False)
                print(int.from_bytes(memory_value, "little", signed=False))

        if(c[0] == FILLED and int_cnt == c[2]+3):
            print('status: ',c[0])
            print('pkt_index: ',c[1])
            print('data_cnt: ',c[2])
            print('data: ',c[3])
            status = TAKEN
                
            write_to_memory(memory,status,0)
            a=(status).to_bytes(4, byteorder='little',signed=True)   
            memory.write(a,0)
            #memory.write(status,0) # <-- This results in an exception
            print('done')
        else:
            print('not ready to load, key: ', ipc_key)
            print('status: ',c[0])
            print('pkt_index: ',c[1])
            print('data_cnt: ',c[2])
            print('data: ',c[3])
except:
    print('Exception: could mean no data')
    pass