from visual import *
import serial

serialFile = '/dev/tty.usbmodem411'
serialPort = serial.Serial(serialFile, 115200)

scene = display(forward=vector(1.0, 0.0, 0.0), up=vector(0.0, 0.0, -1.0))

forward = arrow(pos=vector(0, 0, 0), axis=vector(0, 0, 0), color=color.blue)
down = arrow(pos=vector(0, 0, 0), axis=vector(0, 0, 0), color=color.green)

while(1):
  rate(50)
  line = serialPort.readline()
  data = line.split(",")
  if(len(data) == 7):
    forward.axis=vector(float(data[0]), float(data[1]), float(data[2]))
    down.axis=vector(float(data[3]), float(data[4]), float(data[5]))
