const int AIN1 = 25;
const int AIN2 = 33;
const int BIN1 = 27;
const int BIN2 = 14;
const int PWMA = 32;
const int PWMB = 12;

void PS4_controll() {
  if (PS4.isConnected()) {
    if (PS4.Right()) {
      Serial.println("Right Button");
      pos[2] += 1;
    }
    if (PS4.Down()) {
      Serial.println("Down Button");
      pos[0] += -1;
    }
    if (PS4.Up()) {
      Serial.println("Up Button");
      pos[0] += 1;
    }
    if (PS4.Left()) {
      Serial.println("Left Button");
      pos[2] += -1;
    }
    if (PS4.Square()) {
      Serial.println("Square Button");
      pos[5] += -1;
    }
    if (PS4.Cross()) {
      Serial.println("Cross Button");
      pos[1] += 1;
    }
    if (PS4.Circle()) {
      Serial.println("Circle Button");
      pos[5] += 1;
    }
    if (PS4.Triangle()) {
      Serial.println("Triangle Button");
      pos[1] += -1;
    }

    if (PS4.L1()) {
      Serial.println("L1 Button");
      pos[4] += 1;
    }
    if (PS4.R1()) {
      Serial.println("R1 Button");
      pos[3] += -1;
    }

    if (PS4.Share()) Serial.println("Share Button");
    if (PS4.Options()) Serial.println("Options Button");
    if (PS4.L3()) Serial.println("L3 Button");
    if (PS4.R3()) Serial.println("R3 Button");

    if (PS4.PSButton()) Serial.println("PS Button");
    if (PS4.Touchpad()) {
      Serial.println("Touch Pad Button");
      for (int i = 0; i < 6; i++) {
        pos[i] = init_pos[i];
      }
    }

    if (PS4.L2()) {
      Serial.printf("L2 button at %d\n", PS4.L2Value());
      pos[4] += -1;
    }
    if (PS4.R2()) {
      Serial.printf("R2 button at %d\n", PS4.R2Value());
      pos[3] += 1;
    }

    int RStickY = map(PS4.RStickY(), -128, 128, -256, 256);
    if (RStickY <= 20 && RStickY >= -20) {
      ledcWrite(0, 0);
    } else {
      ledcWrite(0, abs(RStickY));
    }
    //回転方向
    if (RStickY >= 0) {
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN1, HIGH);
    } else {
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN1, LOW);
    }

    2int LStickY = map(PS4.LStickY(), -128, 128, -256, 256);
    if (LStickY <= 20 && LStickY >= -20) {
      ledcWrite(0, 0);
    } else {
      ledcWrite(0, abs(LStickY));
    }
    //回転方向
    if (LStickY >= 0) {
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN1, HIGH);
    } else {
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN1, LOW);
    }
  }
}