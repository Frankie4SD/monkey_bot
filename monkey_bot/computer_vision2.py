import serial
import time

arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)

while True:
    arduino.write(b"300\n")  # send fake x-position
    print("Sent 300")
    time.sleep(1)
