#!/usr/bin/env python
#coding: utf-8

## import the serial library
import serial

## Boolean variable that will represent
## whether or not the arduino is connected
connected = False

## establish connection to the serial port that your arduino
## is connected to.

locations=['/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3']

for device in locations:
    try:
        print "Trying...",device
        ser = serial.Serial(device, 115200)
        break
    except:
        print "Failed to connect on",device

## loop until the arduino tells us it is ready
while not connected:
    serin = ser.read()
    connected = True
print "Connected"
f = open('serial.txt','w')
try:
    while 1:
        if ser.inWaiting():
            x=ser.read()
            f.write(x)
except KeyboardInterrupt:
    pass
except:
    import traceback
    traceback.print_exc()
    pass
## close the serial connection and text file
f.close()
ser.close()
