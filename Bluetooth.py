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