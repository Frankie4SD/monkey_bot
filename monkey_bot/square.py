import serial
import time
from move_regular import Movement


monkey_bot = Movement()

monkey_bot.surge(1,5)
monkey_bot.stop(False)
