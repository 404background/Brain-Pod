#include <SoftwareSerial.h>
SoftwareSerial mySerial(0, 1);

char mes;
int switch1 = D6;
int switch2 = D5;

int AIN1 = D8;     // 1つ目のDCモーターの制御
int AIN2 = D7;     // 1つ目のDCモーターの制御
int BIN1 = D9;     // 2つ目のDCモーターの制御
int BIN2 = D10;     // 2つ目のDCモーターの制御
//int PWMA = 9;     // 1つ目のDCモーターの回転速度
//int PWMB = 10;     // 2つ目のDCモーターの回転速度

void setup() {
  Serial.begin(9600);
  Serial.println("Serial connected");
  mySerial.begin(9600);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}

void loop() {
  /*
  if (Serial.available() > 0) {
    mes = Serial.read();
    Serial.print(mes);
    mySerial.print(mes);
  }
  */
  if (mySerial.available() > 0) {
    while (mySerial.available() > 0) {
      mes = mySerial.read();
      Serial.print(mes);
      Move(mes);
    }
  } else if (digitalRead(switch1) == 0) {
    Move('l');
    Serial.println("MOVE:L");
  } else if (digitalRead(switch2) == 0) {
    Move('r');
    Serial.println("MOVE:R");
  } else {
    Move(' ');
    Serial.println("MOVE:not");
  }
}

void Move(int i) {
  switch (i) {
    case ' ': //停止
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, HIGH);
      break;
    case 'r': // 右回転
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      break;
    case 'l': // 左回転
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
      break;
  }
}
