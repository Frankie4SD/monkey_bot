import serial
import time

# --- Setup ---
try:
    arduino = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    time.sleep(2)  # let Arduino reset
    print("✅ Connected to Arduino.")
except Exception as e:
    print(f"❌ Failed to connect to Arduino: {e}")
    exit(1)

def send_continuous(m1, m2, duration, freq=10):
    """
    Continuously send motor commands for 'duration' seconds.
    freq = how many times per second to send (Hz)
    """
    cmd = f"{m1},{m2}\n".encode()
    end_time = time.time() + duration
    delay = 1.0 / freq

    while time.time() < end_time:
        arduino.write(cmd)
        time.sleep(delay)
    print(f"Sent continuously: {m1},{m2} for {duration}s")

try:
    for i in range(4):
        print(f"\n--- Loop {i+1}/4 ---")

        # Move forward 0.5s
        send_continuous(255, 255, duration=0.5)

        # Stop 1s
        send_continuous(0, 0, duration=1)

        # Turn left 0.5s
        send_continuous(255, -255, duration=0.5)

        # Stop 1s
        send_continuous(0, 0, duration=1)

    print("\n✅ Done moving in square pattern.")

finally:
    # Always stop motors at the end
    send_continuous(0, 0, duration=0.5)
    arduino.close()
    print("🔌 Connection closed, motors stopped.")
