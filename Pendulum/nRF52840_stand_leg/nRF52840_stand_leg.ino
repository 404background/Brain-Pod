#include "LSM6DS3.h"
#include <Wire.h>
#include <Kalman.h>  // Source: https://github.com/TKJElectronics/KalmanFilter
#include <Servo.h>
#include <Adafruit_MCP23X17.h>

#define RESTRICT_PITCH  // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

int LEDR = LED_RED;
int LEDG = LED_GREEN;
int LEDB = LED_BLUE;

Adafruit_MCP23X17 mcp;

/*加速度センサー*****************************************************************/
LSM6DS3 myIMU(I2C_MODE, 0x6A);
Kalman kalmanX;  // Create the Kalman instances
Kalman kalmanY;

double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;
double pitch, roll;
double gyroXrate, gyroYrate;
double dt;

double gyroXangle, gyroYangle;  // Angle calculate using the gyro only
double compAngleX, compAngleY;  // Calculated angle using a complementary filter
double kalAngleX, kalAngleY;    // Calculated angle using a Kalman filter

uint32_t timer;
/********************************************************************************/

/*モーター周り********************************************************************/
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
int p1, p2, p3, p4;

int AIN1 = 0;  // 1つ目のDCモーターの制御
int AIN2 = 1;  // 1つ目のDCモーターの制御
int BIN1 = 2;  // 2つ目のDCモーターの制御
int BIN2 = 3;  // 2つ目のDCモーターの制御
int PWMA = 0;  // 1つ目のDCモーターの回転速度
int PWMB = 6;  // 2つ目のDCモーターの回転速度
/*********************************************************************************/

void setup() {
  digitalWrite(LEDR, LOW);
  Serial.begin(115200);
  mcp.begin_I2C(0x20);
  for (int i = 0; i <= 3; i++) {
    mcp.pinMode(i, OUTPUT);
    mcp.digitalWrite(i, LOW);
  }
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  Move(' ', 0);

  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }
  delay(100);  // Wait for sensor to stabilize

  accX = myIMU.readFloatAccelX();
  accY = myIMU.readFloatAccelY();
  accZ = myIMU.readFloatAccelZ();

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH  // Eq. 25 and 26
  roll = atan2(accY, accZ) * RAD_TO_DEG;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else  // Eq. 28 and 29
  roll = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  kalmanX.setAngle(roll);  // Set starting angle
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;

  timer = micros();


  servo1.attach(7);
  servo2.attach(8);
  servo3.attach(9);
  servo4.attach(10);
  p1 = 90;
  p2 = p1;
  p3 = 90;
  p4 = p3;
  servo1.write(p1);
  servo2.write(p2);
  servo3.write(p3);
  servo4.write(p4);

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
}

void loop() {
  /*
  p1 = map(val1, 0, 1023, 0, 270);
  p2 = 180 - p1;
  p3 = map(val2, 0, 1023, 0, 270);
  p4 = 180 - p3;
  */
  servo1.write(p1);
  servo2.write(p2);
  servo3.write(p3);
  servo4.write(p4);

  accX = myIMU.readFloatAccelX();
  accY = myIMU.readFloatAccelY();
  accZ = myIMU.readFloatAccelZ();
  gyroX = myIMU.readFloatGyroX();
  gyroY = myIMU.readFloatGyroY();
  gyroZ = myIMU.readFloatGyroZ();

  dt = (double)(micros() - timer) / 1000000;  // Calculate delta time
  timer = micros();

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH  // Eq. 25 and 26
  roll = atan2(accY, accZ) * RAD_TO_DEG;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else  // Eq. 28 and 29
  roll = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  gyroXrate = gyroX / 131.0;  // Convert to deg/s
  gyroYrate = gyroY / 131.0;  // Convert to deg/s

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);  // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate;  // Invert rate, so it fits the restriced accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);  // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate;                           // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);  // Calculate the angle using a Kalman filter
#endif

  gyroXangle += gyroXrate * dt;  // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;

  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll;  // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;


  /* Print Data */
  Serial.print(accX);
  Serial.print("\t");
  Serial.print(accY);
  Serial.print("\t");
  Serial.print(accZ);
  Serial.print("\t");

  Serial.print(gyroX);
  Serial.print("\t");
  Serial.print(gyroY);
  Serial.print("\t");
  Serial.print(gyroZ);
  Serial.print("\t");

  Serial.print("\t");

  Serial.print(roll);
  Serial.print("\t");
  Serial.print(gyroXangle);
  Serial.print("\t");
  Serial.print(compAngleX);
  Serial.print("\t");
  Serial.print(kalAngleX);
  Serial.print("\t");

  Serial.print("\t");

  Serial.print(pitch);
  Serial.print("\t");
  Serial.print(gyroYangle);
  Serial.print("\t");
  Serial.print(compAngleY);
  Serial.print("\t");
  Serial.print(kalAngleY);
  Serial.print("\t");

  Serial.print("\r\n");

  if (Serial.available()) {
    char cmd = Serial.read();
    Serial.println(cmd);
    Move(cmd, 50);
  }

  if (kalAngleY >= 30) {
    Move('s', 200);
    p1 = 90 - abs(kalAngleY);
    p2 = 90 + abs(kalAngleY);
  } else if (kalAngleY >= 10) {
    Move('s', 50);
    p1 = 90 - abs(kalAngleY);
    p2 = 90 + abs(kalAngleY);
/********************************************************/
  } else if (kalAngleY <= -10) {
    Move('w', 200);
    p1 = 90 + abs(kalAngleY);
    p2 = 90 - abs(kalAngleY);
/********************************************************/
  } else if (kalAngleY <= -30) {
    Move('w', 200);
    p1 = 90 + abs(kalAngleY);
    p2 = 90 - abs(kalAngleY);
  } else {
    Move(' ', 0);
    p1 = 90;
    p2 = 90;
    p3 = 90;
    p4 = 90;
  }
}

void Move(int i, int x) {
  mcp.begin_I2C(0x20);
  analogWrite(PWMA, x);
  analogWrite(PWMB, x);
  switch (i) {
    case ' ':  //停止
      mcp.digitalWrite(AIN1, LOW);
      mcp.digitalWrite(AIN2, LOW);
      mcp.digitalWrite(BIN1, LOW);
      mcp.digitalWrite(BIN2, LOW);
      break;
    case 'd':  // 右回転
      mcp.digitalWrite(AIN1, HIGH);
      mcp.digitalWrite(AIN2, LOW);
      mcp.digitalWrite(BIN1, HIGH);
      mcp.digitalWrite(BIN2, LOW);
      break;
    case 'a':  // 左回転
      mcp.digitalWrite(AIN1, LOW);
      mcp.digitalWrite(AIN2, HIGH);
      mcp.digitalWrite(BIN1, LOW);
      mcp.digitalWrite(BIN2, HIGH);
      break;
    case 'w':  // 前進
      mcp.digitalWrite(AIN1, HIGH);
      mcp.digitalWrite(AIN2, LOW);
      mcp.digitalWrite(BIN1, LOW);
      mcp.digitalWrite(BIN2, HIGH);
      break;
    case 's':  // 後進
      mcp.digitalWrite(AIN1, LOW);
      mcp.digitalWrite(AIN2, HIGH);
      mcp.digitalWrite(BIN1, HIGH);
      mcp.digitalWrite(BIN2, LOW);
      break;
  }
}
