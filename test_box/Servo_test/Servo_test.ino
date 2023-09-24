#include <ESP32Servo.h>
#include "Servo_setup.h"
#include "Switch.h"

void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  Servo_setPeriodHertz();
  Servo_attach();
  Swtich_setup();
}

void loop() {
  Switch_judge();
  Switch_debug();

  Servo_range();
  Servo_write();
  Servo_debug();
}
