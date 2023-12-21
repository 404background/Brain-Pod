int minUs = 100;
int maxUs = 3000;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
ESP32PWM pwm;

const int servoPin1 = 4;
const int servoPin2 = 16;
const int servoPin3 = 17;
const int servoPin4 = 5;
const int servoPin5 = 18;
const int servoPin6 = 19;
int servoPin[] = { servoPin1, servoPin2, servoPin3, servoPin4,
                   servoPin5, servoPin6 };
int pos[] = { 180, 0, 105, 0, 0, 90 };
const int init_pos[] = { 180, 0, 105, 0, 0, 90 };

void Servo_write() {
  servo1.write(pos[0]);
  servo2.write(pos[1]);
  servo3.write(pos[2]);
  servo4.write(pos[3]);
  servo5.write(pos[4]);
  servo6.write(pos[5]);
}

void Servo_setPeriodHertz() {
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);
  servo5.setPeriodHertz(50);
  servo6.setPeriodHertz(50);
}

void Servo_range() {
  for (int i = 0; i < 6; i++) {
    if (pos[i] > 180) {
      pos[i] = 180;
    } else if (pos[i] < 0) {
      pos[i] = 0;
    }
  }
}

void Servo_attach() {
  servo1.attach(servoPin[0], minUs, maxUs);
  servo2.attach(servoPin[1], minUs, maxUs);
  servo3.attach(servoPin[2], minUs, maxUs);
  servo4.attach(servoPin[3], minUs, maxUs);
  servo5.attach(servoPin[4], minUs, maxUs);
  servo6.attach(servoPin[5], minUs, maxUs);
  pwm.attachPin(27, 10000);
}

void Servo_detach() {
  servo1.detach();
  servo2.detach();
  servo3.detach();
  servo4.detach();
  servo5.detach();
  servo6.detach();
  pwm.detachPin(27);
}

void Servo_debug() {
  Serial.print("Servo:");
  Serial.print(pos[0]);
  Serial.print("  |  ");
  Serial.print(pos[1]);
  Serial.print("  |  ");
  Serial.print(pos[2]);
  Serial.print("  |  ");
  Serial.print(pos[3]);
  Serial.print("  |  ");
  Serial.print(pos[4]);
  Serial.print("  |  ");
  Serial.print(pos[5]);
  Serial.println();
}