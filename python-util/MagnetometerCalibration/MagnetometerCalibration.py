from __future__ import print_function
from visual import *
import serial

serialFile = '/dev/ttyUSB0'
serialPort = serial.Serial(serialFile, 115200)

scene = display(forward=vector(1.0, 0.0, 0.0), up=vector(0.0, 0.0, -1.0))

x = arrow(pos=vector(0, 0, 0), axis=vector(1, 0, 0), color=color.blue)
y = arrow(pos=vector(0, 0, 0), axis=vector(0, 1, 0), color=color.blue)
z = arrow(pos=vector(0, 0, 0), axis=vector(0, 0, 1), color=color.blue)

points = points(pos=[], color=color.green)

while(1):
  rate(50)
  try:
    line = serialPort.readline()
    data = line.split(",")
    points.append(pos=(float(data[0]), float(data[1]), float(data[2])))
    print(line, end='')
  except:
    pass
