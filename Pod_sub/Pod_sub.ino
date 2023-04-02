//A0:バッテリー電圧確認用（https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/）

#include "BNO055.h"
#include "SoftwareSerial.h"

SoftwareSerial mySerial;

char cmd;     //for switch-case
uint32_t Vbatt = 0;
float Vbattf = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(A0, INPUT);         // ADC
  BNO055_setup();
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
  }
  if (mySerial.available()) {
    cmd = mySerial.read();
  }

  switch (cmd) {
    case '1':
      BNO055();
      break;
      
/*Check battery voltage****************************************************************************/
    case '0':     
      for (int i = 0; i < 16; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0); // ADC with correction
      }
      Vbattf = 2 * Vbatt / 16 / 1000.0;     // attenuation ratio 1/2, mV --> V
      Serial.println(Vbattf, 3);
      mySerial.println(Vbattf, 3);
      break;
    default:
      delay(100);
  }
}
