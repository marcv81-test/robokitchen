from __future__ import print_function
from visual import *
import serial

serialFile = '/dev/ttyUSB0'
serialPort = serial.Serial(serialFile, 115200)

scene = display(forward=vector(1.0, 0.0, 0.0), up=vector(0.0, 0.0, -1.0), range=2)

x = arrow(pos=vector(0, 0, 0), axis=vector(1, 0, 0), color=color.blue)
y = arrow(pos=vector(0, 0, 0), axis=vector(0, 1, 0), color=color.blue)
z = arrow(pos=vector(0, 0, 0), axis=vector(0, 0, 1), color=color.blue)

points = points(pos=[], color=color.green)

while(1):
  rate(50)
  try:
    line = serialPort.readline()
    data = line.split(":")
    key = data[0]
    values = data[1].split(",")
    if(key == 'Magnet'):
      points.append(pos=(float(values[0]), float(values[1]), float(values[2])))
      print(data[1], end='')
  except:
    pass
