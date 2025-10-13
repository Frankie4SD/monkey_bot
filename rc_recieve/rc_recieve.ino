// ---------------------- RC Receiver ----------------------
const int ch1Pin = 2;   // CH1 from ER6
const int ch2Pin = 3;   // CH2 from ER6

volatile unsigned long ch1Start, ch2Start;
volatile int ch1Val = 1500, ch2Val = 1500; // defaults
volatile unsigned long lastCh1Update = 0;
volatile unsigned long lastCh2Update = 0;

// ---------------------- Serial Override ----------------------
const int overridePin = 11; // D11 digital input
const int OVERRIDE_THRESHOLD = 1000; // us, min HIGH pulse width to trigger
bool serialOverridePin = false;

// ---------------------- Motors (BTS7960) ----------------------
const int M1_RPWM = 5;
const int M1_LPWM = 6;
const int M2_RPWM = 9;
const int M2_LPWM = 10;

// Serial control state
int serialM1 = 0, serialM2 = 0;
unsigned long lastSerial = 0;
const unsigned long serialTimeout = 500; // ms
bool serialAllowed = false;

// ---------------------- ISR Functions ----------------------
void ISR_ch1() {
  if (digitalRead(ch1Pin) == HIGH) ch1Start = micros();
  else {
    ch1Val = micros() - ch1Start;
    lastCh1Update = millis();
  }

  // Read D11 for serial override
  if (digitalRead(11) == HIGH) serialAllowed = true;
  else serialAllowed = false;
}

void ISR_ch2() {
  if (digitalRead(ch2Pin) == HIGH) ch2Start = micros();
  else {
    ch2Val = micros() - ch2Start;
    lastCh2Update = millis();
  }
}

// ---------------------- Motor Control Function ----------------------
void setBTS7960(int RPWM, int LPWM, int value) {
  int pwm = abs(value);
  if (pwm > 255) pwm = 255;
  if (value > 0) {
    analogWrite(RPWM, pwm);
    analogWrite(LPWM, 0);
  } else if (value < 0) {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, pwm);
  } else {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, 0);
  }
}

// ---------------------- Setup ----------------------
void setup() {
  Serial.begin(115200);

  pinMode(ch1Pin, INPUT);
  pinMode(ch2Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ch1Pin), ISR_ch1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ch2Pin), ISR_ch2, CHANGE);

  pinMode(M1_RPWM, OUTPUT);
  pinMode(M1_LPWM, OUTPUT);
  pinMode(M2_RPWM, OUTPUT);
  pinMode(M2_LPWM, OUTPUT);

  pinMode(overridePin, INPUT);

  Serial.println("RC + Serial Motor Control Ready");
  Serial.println("Serial commands active only if CH2 > 1900 OR D11 HIGH pulse");
}

// ---------------------- Main Loop ----------------------
void loop() {
  int m1Val, m2Val;

  // ------------------ Check Serial Override via pulseIn ------------------
  //unsigned long pulseWidth = pulseIn(overridePin, HIGH, 30000); // timeout 30ms
  //if (pulseWidth > OVERRIDE_THRESHOLD) serialOverridePin = true;
  //else serialOverridePin = false;

  // ------------------ Read Serial Input ------------------
  if (serialAllowed && Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int commaIndex = input.indexOf(',');
    if (commaIndex != -1) {
      serialM1 = constrain(input.substring(0, commaIndex).toInt(), -255, 255);
      serialM2 = constrain(input.substring(commaIndex + 1).toInt(), -255, 255);
      lastSerial = millis();
      Serial.print("SERIAL -> M1: "); Serial.print(serialM1);
      Serial.print("  M2: "); Serial.println(serialM2);
    } else {
      Serial.println("Invalid format. Use: motor1,motor2");
    }
  }

  // ------------------ Determine Motor Values ------------------
  if (serialAllowed && millis() - lastSerial <= serialTimeout) {
    m1Val = serialM1;
    m2Val = serialM2;
  } else {
    // RC Failsafe: stop motors if signal lost
    if ((millis() - lastCh1Update > 200) || (millis() - lastCh2Update > 200)) {
      m1Val = 0;
      m2Val = 0;
    } else {
      int throttle = constrain(map(ch2Val, 1000, 2000, -255, 255), -255, 255);
      int steering = constrain(map(ch1Val, 1000, 2000, -255, 255), -255, 255);
      m1Val = constrain(throttle - steering, -255, 255);
      m2Val = constrain(throttle + steering, -255, 255);
    }
  }

  // ------------------ Apply to Motors ------------------
  setBTS7960(M1_RPWM, M1_LPWM, m1Val);
  setBTS7960(M2_RPWM, M2_LPWM, m2Val);

  // Optional: debug output
  Serial.print("CH1: "); Serial.print(ch1Val);
  Serial.print(" | CH2: "); Serial.print(ch2Val);
  Serial.print(" | SerialAllowed: "); Serial.println(serialAllowed);

  delay(50); // ~20Hz update
}
