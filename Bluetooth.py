<<<<<<< HEAD
#from bluedot import BlueDot
#bd = BlueDot()
#bd.wait_for_press()
#print("You pressed the blue dot!")
from bluedot.btcomm import BluetoothServer
from signal import pause

def data_received(data):
    print(data)
    s.send(data)

s = BluetoothServer(data_received)
pause()
=======
import serial
<<<<<<< HEAD
#import bluetooth
#ser = serial.Serial('/dev/rfcomm0', timeout=1, baudrate=115000)
=======
<<<<<<< HEAD
ser = serial.Serial('/dev/rfcomm0', timeout=1, baudrate=115000)
ser.flushInput();
ser.flushOutput();
   
while True:
    out = ser.readline().decode()
    if out!='' : print (out)
    if out == 'exit': break
#with open('/dev/ttyS0','w',1) as f:
#     f.write('hello from python!')
=======
import bluetooth
#ser = serial.Serial('/dev/ttyS0', timeout=1, baudrate=115000)
>>>>>>> 82cd4db7432b15021e72cc776481975eccf9c8f8
#ser.flushInput();
#ser.flushOutput();
   
#while True:
#    out = ser.readline().decode()
#    if out!='' : print (out)
#    if out == 'exit': break
with open('/dev/rfcomm0','w',1) as f:
     f.write('\'hello from python!')
>>>>>>> 46f979bcb53e3ff8ee293100a7cfcdbbbc2e8927
>>>>>>> 1fa9f9a2e49fe1f16da2224ecdc9479c2f739fc2
