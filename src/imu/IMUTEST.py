# SPDX-FileCopyrightText: Copyright (c) 2020 Bryan Siepert for Adafruit Industries
#
# SPDX-License-Identifier: MIT
import time
import board
import adafruit_lsm6ds
import adafruit_lis3mdl
from adafruit_lsm6ds import Rate, AccelRange, GyroRange



i2c = board.I2C()  # uses board.SCL and board.SDA
lsm = adafruit_lsm6ds.LSM6DSOX(i2c)
lis = adafruit_lis3mdl.LIS3MDL(i2c)
lsm.accelerometer_range = AccelRange.RANGE_2G
accxoff = -0.2093
accyoff =  0.0059
acczoff =  9.8261
gyroxoff = 0.5425
gyroyoff = 0.3194
gyrozoff = 0.1269
magxoff = 52.6966
magyoff = 12.3063
magzoff = 26.0596

def getAcc():
    accx, accy, accz = lsm.acceleration
    #accx -= accxoff
    #accy -= accyoff
    #accz -= acczoff
    return accx, accy, accz

def getGyro():
    gyrox, gyroy, gyroz = lsm.gyro
    gyrox -= gyroxoff
    gyroy -= gyroyoff
    gyroz -= gyrozoff
    return gyrox, gyroy, gyroz

def getMagnetic():
    magx, magy, magz = lis.magnetic
    magx -= magxoff
    magy -= magyoff
    magz -= magzoff
    return magx, magy, magz
while True:
    accx, accy, accz = getAcc()
    print(accx, accy, accz)
    if accx > 1:
        print("x did it")
        break
    if accy > 1:
        print("y did it")
        break
    time.sleep(.1)
    
