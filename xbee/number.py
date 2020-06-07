import matplotlib.pyplot as plt
import numpy as np
import serial
import struct #bytes and float convert

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
number=[]
timestamp=[]
while 1:
    line=s.readline()
    if '/state/run' in line.decode():
        count=float(s.readline().decode().strip())
        time=float(s.readline().decode().strip())
        if time>20:
            break
        number.append(count)
        timestamp.append(time)

plt.plot(timestamp,number)
plt.xlim(0,20)
plt.ylim(0,12)
plt.xlabel('timestamp')
plt.ylabel('number')
plt.show()
