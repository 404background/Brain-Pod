#include "ATP3012.h"
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);


void ATP3012(char str) {
  Wire.beginTransmission(0x2E); // transmit to device 0x2E
  //for(int i = 0; i <= str.length(); i++) {
//  Wire.write(&str);        //send message
  
  Wire.write(0x0D);           //[CR]
  Wire.endTransmission();
}

void ATP3012_Serial() {
  char mes = "";
  while (mySerial.available() > 0) {
    mes = mySerial.read();
  }
}
