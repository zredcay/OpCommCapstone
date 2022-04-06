from bluedot.btcomm import BluetoothServer  # imports the Bluetooth Server library from bluedot
from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
import sys
sys.path.append('/home/pi/OpCommCapstone/src/imu')
import IMU
sys.path.append('/home/pi/OpCommCapstone/src/sharedMemory')
import shm

def data_received(data):  # defines the data_received function which is passed to the BTServer
    print(data)
    
    # write jeff's data to shm
    shm.shmwrite(data, 1)
    
    accx, accy, accz = IMU.getAcc()  # break the acceleration tuple into its respective parts
    gyrox, gyroy, gyroz = IMU.getGyro()  # break the gyro tuple into its respective parts
    magx, magy, magz = IMU.getMagnetic()  # break the magnetic tuple into its respective parts
    
    # assign data to val variable
    val = str(accx) + ' ' + str(accy) + ' ' + str(accz) + ' ' + str(gyrox) + ' ' + str(gyroy) + ' ' + str(gyroz) + ' ' + str(magx) + ' ' + str(magy) + ' ' + str(magz)
    
    # write source's data to shm
    shm.shmwrite(val,0)

    # via the server connection send the following, the variables need to be cast as strings to encode properly
    s.send(val)   
        
s = BluetoothServer(data_received)  # creates the BTServer object and passes it the data function

pause()