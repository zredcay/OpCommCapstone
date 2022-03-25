import time  # needed for a timer to be used
from bluedot.btcomm import BluetoothClient  # imports the Bluetooth Client library from bluedot
from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
import IMU

def data_received(data):  # defines the data_received function which is passed to the BTClient
    print(data)
    
c = BluetoothClient("raspberrypi", data_received)  # creates the BTClient object and pass it the desired device and data function

while True:
    accx, accy, accz = IMU.getAcc()  # break the acceleration tuple into its respective pieces
    gyrox, gyroy, gyroz = IMU.getGyro()  # break the gyro tuple into its respective pieces
    magx, magy, magz = IMU.getMagnetic()  # break the magentic tuple into its respective pieces

    # via the client connection send the following, the variable need to be cast as strings to encode correctly, needs to be one send command otherwise it will double send data
    c.send('magx: ' + str(magx) + ' magy: ' + str(magy) + ' magz: ' + str(magz) + '\n' + 'accx: ' + str(accx) + ' accy: ' + str(accy) + ' accz: ' + str(accz) + '\n' + 'gyrox: ' + str(gyrox) + ' gyroy: ' + str(gyroy) + ' gyroz: ' + str(gyroz) + '\n')
    
    time.sleep(5.0)

pause()