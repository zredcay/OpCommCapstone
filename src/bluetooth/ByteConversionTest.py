from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
import sys
sys.path.append('/home/capstone/OpCommCapstone/src/sharedMemory')
import shm

def testConversion():

    data = str(2.22) + ' ' + str(1.111) + ' ' + str(3.33333) + ' ' + str(4.44444) + ' ' + str(5.55555) + ' ' + str(6.6666) + ' ' + str(7.77777) + ' ' + str(8.888888) + ' ' + str(9.99999)

    print(data)
    
    shm.shmwrite(data, 1)
     # assign data to val variable
    val =str(2.22) + ' ' + str(1.111) + ' ' + str(3.33333) + ' ' + str(4.44444) + ' ' + str(5.55555) + ' ' + str(6.6666) + ' ' + str(7.77777) + ' ' + str(8.888888) + ' ' + str(9.99999)
    
    # write source's data to shm
    shm.shmwrite(val,0)
    print(val)
    pause()
    print('done')
    
testConversion()