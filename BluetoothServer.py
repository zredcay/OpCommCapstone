import time  # needed for a timer to be used
import board  # needed to set up the I2C
import adafruit_lis3mdl  # imports the LIS library

from bluedot.btcomm import BluetoothServer  # imports the Bluetooth Server library from bluedot
from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
from adafruit_lsm6ds.lsm6dsox import LSM6DSOX  # imports the LSM library from the LSM chip family

i2c = board.I2C()  # uses board.SCL and board.SDA
lsm = LSM6DSOX(i2c)  # creates the objet for the LSM allowing for data to be gathered
lis = adafruit_lis3mdl.LIS3MDL(i2c)  # creates the object for the LIS allowing for data to be gathered

def data_received(data):  # defines the data_received function which is passed to the BTServer
    print(data)
    
    accx, accy, accz = lsm.acceleration  # break the acceleration tuple into its respective parts
    gyrox, gyroy, gyroz = lsm.gyro  # break the gyro tuple into its respective parts
    magx, magy, magz = lis.magnetic  # break the magnetic tuple into its respective parts

    # via the server connection send the following, the variables need to be cast as strings to encode properly
    s.send('accx: ' + str(accx) + ' accy: ' + str(accy) + ' accz: ' + str(accz) + '\n' + 'gyrox: ' + str(gyrox) + ' gyroy: ' + str(gyroy) + ' gryoz: ' + str(gyroz) + '\n' + 'magx: ' + str(magx) + ' magy: ' + str(magy) + ' magz: ' + str(magz) + '\n')   
        
s = BluetoothServer(data_received)  # creates the BTServer object and passes it the data function

pause()