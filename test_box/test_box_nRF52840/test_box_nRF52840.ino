#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
int pos1 = A0;
int pos2 = A1;

int p1, p2, p3, p4;

void setup() {
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(3);
  servo4.attach(2);
  Serial.begin(115200);
  Serial.println("Ready.");

  pinMode(pos1, INPUT);
  pinMode(pos2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  p1 = 0;
  p2 = 0;
  p3 = 0;
  p4 = 0;
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    Serial.println(cmd);
    switch (cmd) {
      case 'a':
        p1 += 1;
        break;
      case 'b':
        if (p1 != 0) p1 -= 1;
        break;
      case 'c':
        p2 += 1;
        break;
      case 'd':
        p2 -= 1;
        break;
      case '5':
        p3 += 1;
        break;
      case '6':
        p3 -= 1;
        break;
    }
    servo1.write(p1);
    servo2.write(p2);
    servo3.write(p3);
  }
  else {
    int val1 = analogRead(pos1);
    int val2 = analogRead(pos2);
    p1 = map(val1, 0, 1023, 0, 270);
    p2 = 180 - p1;
    p3 = map(val2, 0, 1023, 0, 270);
    p4 = 180 - p3;
    Serial.print(p1);
    Serial.print(" | ");
    Serial.print(p2);
    Serial.print(" | ");
    Serial.println(p3);

    servo1.write(p1);
    servo2.write(p2);
    servo3.write(p3);
    servo4.write(p4);
  }
  
}
