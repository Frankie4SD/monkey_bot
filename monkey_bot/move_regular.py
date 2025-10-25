import serial
import time






class Movement():
    def __init__(self):
        # --- Setup ---
        self.arduino = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
        time.sleep(2)  # let Arduino reset

    def send_to_arduino(self,m1, m2, delay=0):
        """Send motor values, optionally wait for some time."""
        cmd = f"{m1},{m2}\n"
        self.arduino.write(cmd.encode())
        #print(f"Sent: {cmd.strip()}")
        if delay > 0:
            time.sleep(delay)

    def surge(self,mag,time):
        # mag = mangitude
        print("surge")
        self.send_to_arduino(-255*mag, 255*mag, delay=time)

    def yaw(self,mag,time):
        # mag = mangitude
        # positive is yawing right
        print("yaw")
        self.send_to_arduino(-255*mag,-255*mag, delay=time)

    def stop(self,time):
        # mag = mangitude
        print("stop")
        self.send_to_arduino(0,0, delay=time)

    def close(self):
        arduino.close()
        print("Serial closed.")

p1 = Movement()
p1.surge(1,3)
p1.stop(1)
p1.yaw(1,3)
p1.stop(1)
p1.yaw(-1,3)
p1.close()


"""
# yaw left
send_to_arduino(255, 255, delay=3)


# yaw right
send_to_arduino(-255,-255, delay=3)

# forward
send_to_arduino(-255, 255, delay=3)


send_to_arduino(0, 0, delay=1)

#Backward
send_to_arduino(255, -255, delay=3)

# Stop for 1 sec
send_to_arduino(0, 0, delay=1)
"""
