#include<Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
int pos1 = A0;
int pos2 = A1;

void setup() {
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(3);
  servo4.attach(2);
  Serial.begin(9600);
  pinMode(pos1, INPUT);
  pinMode(pos2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int val1 = analogRead(pos1);
  int val2 = analogRead(pos2);
  int p1 = map(val1, 0, 1023, 0, 180);
  int p2 = 180-p1;
  int p3 = map(val2, 0, 1023, 0, 180);
  int p4 = 180-p3;

  servo1.write(p1);
  servo2.write(p2);
  servo3.write(p3);
  servo4.write(p4);
}
