# SPDX-FileCopyrightText: Copyright (c) 2020 Bryan Siepert for Adafruit Industries
#
# SPDX-License-Identifier: MIT
import time
import board
from adafruit_lsm6ds.lsm6dsox import LSM6DSOX
import adafruit_lis3mdl


i2c = board.I2C()  # uses board.SCL and board.SDA
lsm = LSM6DSOX(i2c)
lis = adafruit_lis3mdl.LIS3MDL(i2c)


def getAcc():
    accx, accy, accz = lsm.acceleration
    return accx, accy, accz

def getGyro():
    gyrox, gyroy, gyroz = lsm.gyro
    return gyrox, gyroy, gyroz

def getMagnetic():
    magx, magy, magz = lis.magnetic
    return magx, magy, magz

