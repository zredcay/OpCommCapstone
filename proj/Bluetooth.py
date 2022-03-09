import serial
ser = serial.Serial('/dev/ttyS0', timeout=1, baudrate=115000)
ser.flushInput();
ser.flushOutput();
   
while True:
    out = ser.readline().decode()
    if out!='' : print (out)