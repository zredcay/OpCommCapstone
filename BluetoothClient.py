import time
import board
# import busio
import adafruit_lis3mdl

from bluedot.btcomm import BluetoothClient
from signal import pause
from adafruit_lsm6ds.lsm6dsox import LSM6DSOX

i2c = board.I2C()  # uses board.SCL and board.SDA
lsm = LSM6DSOX(i2c)

#i2c = busio.I2C(board.SCL, board.SDA)
lis = adafruit_lis3mdl.LIS3MDL(i2c)

def data_received(data):
    print(data)
    

c = BluetoothClient("raspberrypi", data_received)
c.send(lsm.acceleration)
c.send(lsm.gyro)
c.send(lis.magnetic)

#probablt need a timer

pause()