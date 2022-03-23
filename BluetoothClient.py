import time  # needed for a timer to be used
import board  # needed to set up the I2C
import adafruit_lis3mdl  # imports the LIS library

from bluedot.btcomm import BluetoothClient  # imports the Bluetooth Client library from bluedot
from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
from adafruit_lsm6ds.lsm6dsox import LSM6DSOX  # imports the LSM library from the LSM chip family

i2c = board.I2C()  # uses board.SCL and board.SDA
lsm = LSM6DSOX(i2c)  # creates the object for the LSM allowing for data to be gathered
lis = adafruit_lis3mdl.LIS3MDL(i2c)  # creates the object for the LIS allowing for data to be gathered

def data_received(data):  # defines the data_received function which is passed to the BTClient
    print(data)
    
c = BluetoothClient("raspberrypi", data_received)  # creates the BTClient object and pass it the desired device and data function

while True:
    accx, accy, accz = lsm.acceleration  # break the acceleration tuple into its respective pieces
    gyrox, gyroy, gyroz = lsm.gyro  # break the gyro tuple into its respective pieces
    magx, magy, magz = lis.magnetic  # break the magentic tuple into its respective pieces

    # via the client connection send the following, the variable need to be cast as strings to encode correctly, needs to be one send command otherwise it will double send data
    c.send('magx: ' + str(magx) + ' magy: ' + str(magy) + ' magz: ' + str(magz) + '\n' + 'accx: ' + str(accx) + ' accy: ' + str(accy) + ' accz: ' + str(accz) + '\n' + 'gyrox: ' + str(gyrox) + ' gyroy: ' + str(gyroy) + ' gyroz: ' + str(gyroz) + '\n')
    
    time.sleep(5.0)

pause()