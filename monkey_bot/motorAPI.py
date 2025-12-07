import serial
import time
"""
Objective: 
the interface to the ardiuno
ensures the commands are mixed correctly for less cognitive load on the user

"""

class Movement():
    def __init__(self):
        # --- Setup ---
        self.arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
        time.sleep(2)  # let Arduino reset

    def send_to_arduino(self,m1, m2, delay=0):
        """Send motor values, optionally wait for some time."""
        cmd = f"{m1},{m2}\n"
        self.arduino.write(cmd.encode())
        #print(f"Sent: {cmd.strip()}")
        if delay != False:
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
