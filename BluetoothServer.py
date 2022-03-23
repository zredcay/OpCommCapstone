import time
import board
# import busio
import adafruit_lis3mdl

from bluedot.btcomm import BluetoothServer
from signal import pause
from adafruit_lsm6ds.lsm6dsox import LSM6DSOX

i2c = board.I2C()  # uses board.SCL and board.SDA
lsm = LSM6DSOX(i2c)

#i2c = busio.I2C(board.SCL, board.SDA)
lis = adafruit_lis3mdl.LIS3MDL(i2c)

def data_received(data):
    print(data)
    
    accx, accy, accz = lsm.acceleration
    gyrox, gyroy, gyroz = lsm.gyro
    magx, magy, magz = lis.magnetic

    s.send('accx: ' + str(accx) + ' ')
    s.send('accy: ' + str(accy) + ' ')
    s.send('accz: ' + str(accz) + ' ')
    s.send('gyrox: ' + str(gyrox) + ' ')
    s.send('gyroy: ' + str(gyroy) + ' ')
    s.send('gryoz: ' + str(gyroz) + ' ')
    s.send('magx: ' + str(magx) + ' ')
    s.send('magy: ' + str(magy) + ' ')
    s.send('magz: ' + str(magz) + ' ')
    s.send('\n')
    
        

s = BluetoothServer(data_received)

pause()