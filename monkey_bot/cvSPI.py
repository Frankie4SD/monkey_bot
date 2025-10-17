from picamera2 import Picamera2
import cv2
import time

# Initialize camera
picam2 = Picamera2()

# Configure preview resolution (adjust as needed)
preview_config = picam2.create_preview_configuration(main={"size": (640, 480)})
picam2.configure(preview_config)

# Start camera
picam2.start()
time.sleep(1)  # Allow sensor to warm up

# Live preview loop
while True:
    frame = picam2.capture_array()
    cv2.imshow("IMX219-160 Camera", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Cleanup
picam2.stop()
cv2.destroyAllWindows()
