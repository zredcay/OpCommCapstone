import time  # needed for a timer to be used
from bluedot.btcomm import BluetoothClient  # imports the Bluetooth Client library from bluedot
from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
import sys
sys.path.append('/home/pi/OpCommCapstone/src/imu')
import IMU
sys.path.append('/home/pi/OpCommCapstone/src/sharedMemory')
import shm


def data_received(data):  # defines the data_received function which is passed to the BTClient
    print(data)
    
    # write source's data to shm
    shm.shmwrite(data, 0)
    
c = BluetoothClient("raspberrypi", data_received)  # creates the BTClient object and pass it the desired device and data function

while True:
    accx, accy, accz = IMU.getAcc()  # break the acceleration tuple into its respective pieces
    gyrox, gyroy, gyroz = IMU.getGyro()  # break the gyro tuple into its respective pieces
    magx, magy, magz = IMU.getMagnetic()  # break the magentic tuple into its respective pieces

    # assign data to data variable
    data = str(accx) + ', ' + str(accy) + ', ' + str(accz) + ', ' + str(gyrox) + ', ' + str(gyroy) + ', ' + str(gyroz) + ', ' + str(magx) + ', ' + str(magy) + ', ' + str(magz)

    # write jeff's data to shm
    shm.shmwrite(data, 1)
    
    # via the client connection send the following, the variable need to be cast as strings to encode correctly, needs to be one send command otherwise it will double send data
    c.send(data)   
    
    time.sleep(5.0)

pause()