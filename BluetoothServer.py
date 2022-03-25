from bluedot.btcomm import BluetoothServer  # imports the Bluetooth Server library from bluedot
from signal import pause  # imports pause function from signal allowing program to idle until a connection is created
import IMU

def data_received(data):  # defines the data_received function which is passed to the BTServer
    print(data)
    
    accx, accy, accz = IMU.getAcc()  # break the acceleration tuple into its respective parts
    gyrox, gyroy, gyroz = IMU.getGyro()  # break the gyro tuple into its respective parts
    magx, magy, magz = IMU.getMagnetic  # break the magnetic tuple into its respective parts

    # via the server connection send the following, the variables need to be cast as strings to encode properly
    s.send('accx: ' + str(accx) + ' accy: ' + str(accy) + ' accz: ' + str(accz) + '\n' + 'gyrox: ' + str(gyrox) + ' gyroy: ' + str(gyroy) + ' gryoz: ' + str(gyroz) + '\n' + 'magx: ' + str(magx) + ' magy: ' + str(magy) + ' magz: ' + str(magz) + '\n')   
        
s = BluetoothServer(data_received)  # creates the BTServer object and passes it the data function

pause()