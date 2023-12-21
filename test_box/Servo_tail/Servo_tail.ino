#include <ESP32Servo.h>
#include <PS4Controller.h>
#include "Servo_setup.h"
#include "Switch.h"
#include "PS4.h"

void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  Servo_setPeriodHertz();
  Servo_attach();
  //Swtich_setup();
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  ledcSetup(0, 12800, 8);
  ledcAttachPin(PWMA, 0);
  ledcSetup(1, 12800, 8);
  ledcAttachPin(PWMB, 1);
  PS4.begin();
}

void loop() {
  //Switch_judge();
  //Switch_debug();

  Servo_range();
  Servo_write();
  Servo_debug();

  PS4_controll();
}
