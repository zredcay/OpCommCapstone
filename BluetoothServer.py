from signal import pause

def data_received(data):
    print(data)
#    i = 1
#    while i < 2:
    s.send(data)
        

s = BluetoothServer(data_received)
pause()