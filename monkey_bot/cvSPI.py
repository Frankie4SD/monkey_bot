import cv2
import numpy as np
from arducam_mini import ArducamMini  # Replace with your actual SPI camera module

camera = ArducamMini()
camera.begin()

while True:
    raw_data = camera.capture_frame()
    if raw_data is None:
        continue

    # Convert raw data to numpy array (assuming RGB565 or similar)
    frame = np.frombuffer(raw_data, dtype=np.uint8)
    frame = frame.reshape((height, width, 2))  # Adjust dimensions and format
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR5652BGR)

    cv2.imshow("SPI Camera", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

camera.end()
cv2.destroyAllWindows()
