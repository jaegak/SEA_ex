#!/usr/bin/env python

import time
import serial

ser = serial.Serial(port = '/dev/ttyUSB0',baudrate=9600)
# ser.port = "/dev/ttyUSB0"
# ser.baudrate = 9600
# ser.open()
if ser.isOpen():
    ser.write('QS\r')
    while 1:
        response = ser.readline()
        print(response)
ser.close()