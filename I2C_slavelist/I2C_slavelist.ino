//https://akizukidenshi.com/catalog/g/gK-16996/
#include <Wire.h>

bool slavePresent(byte adr) {
  Wire.beginTransmission(adr);
  return(Wire.endTransmission() == 0);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  Serial.println("I2C slave device list.");
  for(byte adr = 1; adr < 127; adr++) {
    if(slavePresent(adr)) {
      if(adr < 16) {
        Serial.print("0");
      }
      Serial.print(adr, HEX);
      Serial.print(" ");
    }
  }
  Serial.println("\nDone");
  delay(5000);
}
