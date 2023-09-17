#include <Wire.h>
#include <ESP32Servo.h>
#include <PS4Controller.h>

/*モーター周り********************************************************************/
Servo servo1;
Servo servo2;
int minUs = 10;
int maxUs = 100000;
int servoHz = 50;

int servoPin1 = 12;
int servoPin2 = 13;
int pos1, pos2;

int AIN1 = 25;  // 1つ目のDCモーターの制御
int AIN2 = 33;  // 1つ目のDCモーターの制御
int PWMA = 32;  // 1つ目のDCモーターの回転速度
int BIN1 = 26;  // 2つ目のDCモーターの制御
int BIN2 = 27;  // 2つ目のDCモーターの制御
int PWMB = 14;  // 2つ目のDCモーターの回転速度
int CIN1 = 0;   // 3つ目のDCモーターの制御
int CIN2 = 2;   // 3つ目のDCモーターの制御
int PWMC = 15;  // 3つ目のDCモーターの回転速度

void Servo_angle_range() {
  if (pos1 > 180) {
    pos1 = 180;
  } else if (pos1 < 0) {
    pos1 = 0;
  }

  if (pos2 > 180) {
    pos2 = 180;
  } else if (pos2 < 0) {
    pos2 = 0;
  }
}

void Move(int _switch, int _pin1, int _pin2, int _speed, int _lcdpin) {
  ledcWrite(_lcdpin, _speed);
  switch (_switch) {
    case ' ':  //停止
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, LOW);
      break;
    case 'd':  // 右回転
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin2, LOW);
      break;
    case 'a':  // 左回転
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, HIGH);
      break;
    case 'w':  // 前進
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, HIGH);
      break;
    case 's':  // 後進
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin2, LOW);
      break;
  }
}

void Move2(int _switch, int _pin1, int _pin2, int _pwmpin, int _on) {
  digitalWrite(_pwmpin, _on);
  switch (_switch) {
    case ' ':  //停止
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, LOW);
      break;
    case 'd':  // 右回転
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin2, LOW);
      break;
    case 'a':  // 左回転
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, HIGH);
      break;
    case 'w':  // 前進
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, HIGH);
      break;
    case 's':  // 後進
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin2, LOW);
      break;
  }
}
/*********************************************************************************/

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
    if (PS4.Right()) Serial.println("Right Button");
    else if (PS4.Down()) {
      pos1 += 1;
      delay(5);
    } else if (PS4.Up()) {
      pos1 += -1;
      delay(5);
    }
    //} else if (PS4.Left()) Serial.println("Left Button");

    //else if (PS4.Square()) Serial.println("Square Button");
    else if (PS4.Cross()) {
      pos2 += -1;
      delay(5);
    }
    //else if (PS4.Circle()) Serial.println("Circle Button");
    else if (PS4.Triangle()) {
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
