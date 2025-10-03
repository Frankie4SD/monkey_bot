import serial
import time

# --- Setup ---
arduino = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
time.sleep(2)  # let Arduino reset

def send_to_arduino(m1, m2, delay=0):
    """Send motor values, optionally wait for some time."""
    cmd = f"{m1},{m2}\n"
    arduino.write(cmd.encode())
    print(f"Sent: {cmd.strip()}")
    if delay > 0:
        time.sleep(delay)


for i in range(1,5):
    #Forward 1  (Start of square)
    send_to_arduino(255, 255, delay=.5)

    # Stop for 1 sec
    send_to_arduino(0, 0, delay=1)

    #Left turn
    send_to_arduino(255, -255, delay=.5)

    # Stop for 1 sec
    send_to_arduino(0, 0, delay=1)
