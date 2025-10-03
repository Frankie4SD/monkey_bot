// ---------------------- RC Receiver ----------------------
const int ch1Pin = 2;   // CH1 from ER6
const int ch2Pin = 3;   // CH2 from ER6

volatile unsigned long ch1Start, ch2Start;
volatile int ch1Val = 1500, ch2Val = 1500; // defaults

void ISR_ch1() {
  if (digitalRead(ch1Pin) == HIGH) ch1Start = micros();
  else ch1Val = micros() - ch1Start;
}
void ISR_ch2() {
  if (digitalRead(ch2Pin) == HIGH) ch2Start = micros();
  else ch2Val = micros() - ch2Start;
}

// ---------------------- Motors (BTS7960) ----------------------
const int M1_RPWM = 5;
const int M1_LPWM = 6;
const int M2_RPWM = 9;
const int M2_LPWM = 10;

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

  Serial.println("RC + Serial Motor Control Ready");
  Serial.println("Type: motor1,motor2  (Example: 120,-100)");
}

// ---------------------- Main Loop ----------------------
void loop() {
  int m1Val, m2Val;

  // ---------------- Serial Input ----------------
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int commaIndex = input.indexOf(',');
    if (commaIndex != -1) {
      m1Val = input.substring(0, commaIndex).toInt();
      m2Val = input.substring(commaIndex + 1).toInt();

      // Clamp values
      if (m1Val > 255) m1Val = 255;
      if (m1Val < -255) m1Val = -255;
      if (m2Val > 255) m2Val = 255;
      if (m2Val < -255) m2Val = -255;

      setBTS7960(M1_RPWM, M1_LPWM, m1Val);
      setBTS7960(M2_RPWM, M2_LPWM, m2Val);

      Serial.print("SERIAL -> M1: "); Serial.print(m1Val);
      Serial.print("  M2: "); Serial.println(m2Val);

      return; // skip RC update this cycle
    } else {
      Serial.println("Invalid format. Use: motor1,motor2");
    }
  }

  // ---------------- RC Input ----------------
  int ch1 = ch1Val; // steering
  int ch2 = ch2Val; // throttle

  int throttle = map(ch2, 1000, 2000, -255, 255);
  int steering = map(ch1, 1000, 2000, -255, 255);

  m1Val = throttle - steering; // Right motor
  m2Val = throttle + steering; // Left motor

  // Clamp values
  if (m1Val > 255) m1Val = 255;
  if (m1Val < -255) m1Val = -255;
  if (m2Val > 255) m2Val = 255;
  if (m2Val < -255) m2Val = -255;

  setBTS7960(M1_RPWM, M1_LPWM, m1Val);
  setBTS7960(M2_RPWM, M2_LPWM, m2Val);

  Serial.print("RC -> CH1: "); Serial.print(ch1);
  Serial.print("  CH2: "); Serial.print(ch2);
  Serial.print("  M1: "); Serial.print(m1Val);
  Serial.print("  M2: "); Serial.println(m2Val);

  delay(50); // ~20 Hz updates
}
