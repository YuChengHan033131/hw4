import serial
import time

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev)
while 1:
    s.write(bytes("/state/run \r", 'UTF-8'))
    time.sleep(1)