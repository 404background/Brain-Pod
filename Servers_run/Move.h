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