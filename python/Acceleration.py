from __future__ import print_function
from visual import *
import serial
import sys

serialFile = sys.argv[1]
serialPort = serial.Serial(serialFile, sys.argv[2])

scene = display(forward=vector(1.0, 0.0, 0.0), up=vector(0.0, 0.0, -1.0), range=2)

accel = arrow(pos=vector(0, 0, 0), axis=vector(0, 0, 0), color=color.green)

while(1):
  rate(50)
  try:
    line = serialPort.readline()
    data = line.split(":")
    key = data[0]
    values = data[1].split(",")
    if(key == 'Accel'):
      accel.axis=vector(float(values[0]), float(values[1]), float(values[2]))
      print(data[1], end='')
  except:
    pass
