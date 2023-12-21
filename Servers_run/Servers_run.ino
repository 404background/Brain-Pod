#include <Wire.h>
#include <ESP32Servo.h>
#include <PS4Controller.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include "Move.h"

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(CIN1, OUTPUT);
  pinMode(CIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(PWMC, OUTPUT);
/*
  ledcSetup(0, 12800, 8);
  ledcAttachPin(PWMA, 0);
  ledcSetup(1, 12800, 8);
  ledcAttachPin(PWMB, 1);
  ledcSetup(2, 12800, 8);
  ledcAttachPin(PWMC, 2);
*/
  Move(' ', AIN1, AIN2, 0, 0);
  Move(' ', BIN1, BIN2, 0, 1);
  Move(' ', CIN1, CIN2, 0, 2);

  Serial.println("Servo setup");
  servo1.attach(servoPin1, minUs, maxUs);
  servo2.attach(servoPin2, minUs, maxUs);
  servo1.setPeriodHertz(servoHz);  // Standard 50hz servo
  servo2.setPeriodHertz(servoHz);  // Standard 50hz servo
  pos1 = 50;
  pos2 = 130;
  servo1.write(pos1);
  servo2.write(pos2);

  Serial.println("PS4 setup");
  PS4.begin();
  const uint8_t* address = esp_bt_dev_get_address();
  char str[100];
  sprintf(str, "ESP32's Bluetooth MAC address is - %02x:%02x:%02x:%02x:%02x:%02x", address[0], address[1], address[2], address[3], address[4], address[5]);
  Serial.println(str);
}

void loop() {
  Servo_angle_range();
  servo1.write(pos1);
  servo2.write(pos2);

  Serial.print(pos1);
  Serial.print("   ");
  Serial.print(pos2);
  Serial.println();

  if (PS4.isConnected()) {
    //if (PS4.Right()) Serial.println("Right Button");
    if (PS4.Down()) {
      pos1 += 1;
      pos2 += -1;
      delay(5);
    } else if (PS4.Up()) {
      pos1 += -1;
      pos2 += 1;
      delay(5);
      // } else if (PS4.Left()) {
      //   pos1 += 1;
      //   delay(5);
    } else if (PS4.Square()) {
      pos1 += -1;
      delay(5);
    } else if (PS4.Cross()) {
      pos2 += -1;
      delay(5);
    } else if (PS4.Circle()) {
      pos1 += 1;
      delay(5);
    } else if (PS4.Triangle()) {
      pos2 += 1;
      delay(5);
    } else if (PS4.L1()) {
      //Move('s', AIN1, AIN2, 128, 0);
      Move2('s', AIN1, AIN2, PWMA, 1);
      //Move('s', BIN1, BIN2, 128, 1);
      Move2('s', BIN1, BIN2, PWMB, 1);
      //Move('w', CIN1, CIN2, 128, 2);
      Move2('w', CIN1, CIN2, PWMC, 1);
      digitalWrite(PWMA, HIGH);
      digitalWrite(PWMB, HIGH);
      digitalWrite(PWMC, HIGH);
    } else if (PS4.R1()) {
      //Move('w', AIN1, AIN2, 128, 0);
      Move2('w', AIN1, AIN2, PWMA, 1);
      //Move('w', BIN1, BIN2, 128, 1);
      Move2('w', BIN1, BIN2, PWMB, 1);
      //Move('s', CIN1, CIN2, 128, 2);
      Move2('s', CIN1, CIN2, PWMC, 1);
      digitalWrite(PWMA, HIGH);
      digitalWrite(PWMB, HIGH);
      digitalWrite(PWMC, HIGH);
    }
    /*
      else if (PS4.Share()) Serial.println("Share Button");
      else if (PS4.Options()) Serial.println("Options Button");
      else if (PS4.L3()) Serial.println("L3 Button");
      else if (PS4.R3()) Serial.println("R3 Button");
*/
    //else if (PS4.PSButton()) Serial.println("PS Button");

    else if (PS4.L2()) {
      Move2('s', CIN1, CIN2, PWMC, 1);
    } else if (PS4.R2()) {
      Move2('w', CIN1, CIN2, PWMC, 1);
    } else {
      Move2(' ', AIN1, AIN2, PWMA, 0);
      Move2(' ', BIN1, BIN2, PWMB, 0);
      Move2(' ', CIN1, CIN2, PWMC, 0);
    }

    //スティックの傾きに合わせて、回転速度を調整
    //Lスティックの反応が悪かったため、Rスティックに変更
    int RStickY = map(PS4.RStickY(), -128, 128, -256, 256);
    if (RStickY <= 20 && RStickY >= -20) {
      Move('w', AIN1, AIN2, 0, 0);
    } else {
      //回転方向
      if (RStickY >= 0) {  //前進
        //Move('w', AIN1, AIN2, abs(RStickY), 0);
        Move2('w', AIN1, AIN2, PWMA, 1);
      } else {  //後退
        //Move('s', AIN1, AIN2, abs(RStickY), 0);
        Move2('s', AIN1, AIN2, PWMA, 1);
      }
    }

    //スティックの傾きに合わせて、回転速度を調整
    int LStickY = map(PS4.LStickY(), -128, 128, -256, 256);
    if (LStickY <= 20 && LStickY >= -20) {
      Move('w', BIN1, BIN2, 0, 1);
    } else {
      //回転方向
      if (LStickY >= 0) {  //back
        //Move('w', BIN1, BIN2, abs(LStickY), 1);
        Move2('w', BIN1, BIN2, PWMB, 1);
      } else {  //forward
        //Move('s', BIN1, BIN2, abs(LStickY), 1);
        Move2('s', BIN1, BIN2, PWMB, 1);
      }
    }

    while (PS4.Touchpad()) {
      Move2(' ', AIN1, AIN2, PWMA, 0);
      Move2(' ', BIN1, BIN2, PWMB, 0);
      Move2(' ', CIN1, CIN2, PWMC, 0);
      pos1 = 50;
      pos2 = 130;
      servo1.write(pos1);
      servo2.write(pos2);
    }
  }
}
