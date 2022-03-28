import numpy as np
import sysv_ipc
import sys


# create shared memory object
ipc_key = 123456


PY_MAJOR_VERSION = sys.version_info[0]


memory = sysv_ipc.SharedMemory(ipc_key, flags=0)
bytes_val = int.to_bytes(5, 1, 'little')

print(bytes_val)
memory.write(bytes_val, offset = 0)
