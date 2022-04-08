from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
import sys
sys.path.append('/home/capstone/OpCommCapstone/src/sharedMemory')
import shm

def testConversion():

    data = str(4.333) + '/0' + str(5.678)
    
    print(data)
    
    shm.shmwrite(data, 1)
     # assign data to val variable
    val = str(1.234) + '/0 ' + str(9.845) 
    
    # write source's data to shm
    shm.shmwrite(val,0)
    print(val)
    pause()
    print('done')
    
testConversion()