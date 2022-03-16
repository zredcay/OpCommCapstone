import serial
#import bluetooth
#ser = serial.Serial('/dev/rfcomm0', timeout=1, baudrate=115000)
#ser.flushInput();
#ser.flushOutput();
   
#while True:
#    out = ser.readline().decode()
#    if out!='' : print (out)
#    if out == 'exit': break
with open('/dev/rfcomm0','w',1) as f:
     f.write('\'hello from python!')
