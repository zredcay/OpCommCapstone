#from bluedot import BlueDot
#bd = BlueDot()
#bd.wait_for_press()
#print("You pressed the blue dot!")
from bluedot.btcomm import BluetoothClient
from signal import pause

def data_received(data):
    print(data)
    #s.send(data)

c = BluetoothClient("raspberrypi", data_received)
c.send("hello world")

pause()