import sysv_ipc

# create shared memory object
ipc_key = 123456

memory = sysv_ipc.SharedMemory(ipc_key, flags=0)
data = 1234
memory.write(data, offset = 0)
