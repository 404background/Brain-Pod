#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <IRremote.h>
#include <Audio.h>
#include <SDHCI.h>
#include <Camera.h>
#include <Wire.h>
#include <RTC.h>
#include "ATP3012.h"


#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8


#define RECEIVER 1
#define SENDER 2
#define ERROR 3
#define BALL_SWITCH 2
#define LED 3
#define RECV_PIN 7
#define SWITCH 0
#define TIME_HEADER 'T' // Header tag for serial time sync message

#define MD1_dir 3
#define MD2_dir 4
#define MD1_PWM 5
#define MD2_PWM 6


Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;


SDClass theSD;
File myFile;
AudioClass *theAudio = AudioClass::getInstance();
bool ErrEnd = false;

static const int32_t recording_time_ms = 10000; /* 10秒 */
static int32_t start_time_ms;

RtcTime rtc;

int disp_wait = 0;    //display waiting...
int disp_count = 0;
int take_picture_count = 0;
int light_value = 0;    //LED value

int m_power = 100;

/**********************************************************************************************/
void Println(String m) {
  Serial.println(m);
  tft.println(m);
  disp_count++;
  if (disp_count >= 8) {
    tft.fillScreen(ILI9341_BLACK);
    yield();
    tft.setCursor(0, 0);
    disp_count = 0;
  }
}

void IRreceive() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    tft.println(results.value);
    irrecv.resume(); // Receive the next value
    disp_count++;
    if (disp_count >= 8) {
      tft.fillScreen(ILI9341_BLACK);
      yield();
      tft.setCursor(0, 0);
      disp_count = 0;
    }
  }
}

void ScreenReset() {
  tft.fillScreen(ILI9341_BLACK);
  //yield();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  disp_count = 0;
}

void Record() {
  theAudio->begin();
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);
  theAudio->initRecorder(AS_CODECTYPE_MP3, "/mnt/sd0/BIN"
                         , AS_SAMPLINGRATE_48000, AS_CHANNEL_MONO);
  if (theSD.exists("Sound.mp3")) {
    theSD.remove("Sound.mp3");
  }
  myFile = theSD.open("Sound.mp3", FILE_WRITE);
  if (!myFile) {
    Serial.println("File open error\n");
    while (1);
  }
  theAudio->startRecorder();
  start_time_ms = millis();
  Serial.println("Start Recording");
  tft.println("Start Recording");

  uint32_t duration_ms = millis() - start_time_ms;
  err_t err = theAudio->readFrames(myFile);

  if (duration_ms > recording_time_ms
      || err != AUDIOLIB_ECODE_OK) {
    theAudio->stopRecorder();
    theAudio->closeOutputFile(myFile);
    theAudio->setReadyMode();
    theAudio->end();
    Serial.println("End Recording");
    tft.println("End Recording");
    delay(1000);
  }
}

void CamCB(CamImage img)
{
  if (img.isAvailable()) {
    img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    tft.drawRGBBitmap(0, 0 /* 開始座標 */
                      , (uint16_t*)img.getImgBuff() /* 画像データ */
                      , 320, 240); /* 横幅、縦幅  */
  }
  else {
    Println("Failed to get video stream image");
  }
}

void printError(enum CamErr err)
{
  Serial.print("Error: ");
  switch (err)
  {
    case CAM_ERR_NO_DEVICE:
      Serial.println("No Device");
      break;
    case CAM_ERR_ILLEGAL_DEVERR:
      Serial.println("Illegal device error");
      break;
    case CAM_ERR_ALREADY_INITIALIZED:
      Serial.println("Already initialized");
      break;
    case CAM_ERR_NOT_INITIALIZED:
      Serial.println("Not initialized");
      break;
    case CAM_ERR_NOT_STILL_INITIALIZED:
      Serial.println("Still picture not initialized");
      break;
    case CAM_ERR_CANT_CREATE_THREAD:
      Serial.println("Failed to create thread");
      break;
    case CAM_ERR_INVALID_PARAM:
      Serial.println("Invalid parameter");
      break;
    case CAM_ERR_NO_MEMORY:
      Serial.println("No memory");
      break;
    case CAM_ERR_USR_INUSED:
      Serial.println("Buffer already in use");
      break;
    case CAM_ERR_NOT_PERMITTED:
      Serial.println("Operation not permitted");
      break;
    default:
      break;
  }
}

void printClock(RtcTime &rtc)
{
  tft.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
             rtc.year(), rtc.month(), rtc.day(),
             rtc.hour(), rtc.minute(), rtc.second());
}

void updateClock()
{
  static RtcTime old;
  RtcTime now = RTC.getTime();

  // Display only when the second is updated
  if (now != old) {
    printClock(now);
    old = now;
  }
}






/**************************************************************************************************/
void setup() {
  Serial.begin(115200);
  Wire.begin();
  tft.begin(40000000);
  irrecv.enableIRIn(); // Start the receiver

  tft.fillScreen(ILI9341_BLACK);
  yield();
  tft.setRotation(3);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("Hello World!");


  Wire.beginTransmission(0x2E); // transmit to device 0x2E
  Wire.write("se'tto./");       //send message
  Wire.write(0x0D);           //[CR]
  Wire.endTransmission();    // stop transmitting

  pinMode(BALL_SWITCH, INPUT_PULLUP);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  analogWrite(LED, 0);

  pinMode(MD1_dir, OUTPUT);
  pinMode(MD2_dir, OUTPUT);
  pinMode(MD1_PWM, OUTPUT);
  pinMode(MD2_PWM, OUTPUT);
  digitalWrite(MD1_PWM, LOW);
  digitalWrite(MD2_PWM, LOW);

  while (!theSD.begin())
  {
    /* wait until SD card is mounted. */
    Println("Insert SD card.");
  }


}

/*****************************************************************************************************/

void loop() {
  while (1) {
    IRreceive();
    switch (results.value) {
      /***************************************************************************************/
      case '1':
      case 16724175:
        Println("Camera on!");
        Wire.beginTransmission(0x2E); // transmit to device 0x2E
        Wire.write("ka'merawo,/ kidoushimasu.");
        Wire.write(0x0D);
        Wire.endTransmission();    // stop transmitting
        tft.fillScreen(ILI9341_BLUE);
        yield();
        tft.setRotation(3); // ディスプレイの向きを設定

        CamErr err;

        Println("Prepare camera");
        err = theCamera.begin(); // カメラの開始
        if (err != CAM_ERR_SUCCESS) {
          printError(err);
        }

        Println("Start streaming");
        err = theCamera.startStreaming(true, CamCB); // カメラのストリーミングを開始
        if (err != CAM_ERR_SUCCESS)
        {
          printError(err);
        }
        Println("Set Auto white balance parameter");
        err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);

        Println("Set still picture format");
        err = theCamera.setStillPictureImageFormat(
                CAM_IMGSIZE_QUADVGA_H,
                CAM_IMGSIZE_QUADVGA_V,
                CAM_IMAGE_PIX_FMT_JPG);
        if (err != CAM_ERR_SUCCESS)
        {
          printError(err);
        }
        /*
                irrecv.decode(&results);
                delay(1000);
        */
        while (results.value != 16753245) {
          IRreceive();
          sleep(1);

          while (digitalRead(SWITCH) == 0) {
            Wire.beginTransmission(0x2E); // transmit to device 0x2E
            Wire.write("#J");       //send message
            Wire.write(0x0D);
            Wire.endTransmission();    // stop transmitting
            CamImage img = theCamera.takePicture();
            if (img.isAvailable())
            {
              /*
                updateClock();
                sprintf(filename, "PICT%04d/%02d/%02d %02d:%02d:%02d.JPG",
                      rtc.year(), rtc.month(), rtc.day(),
                      rtc.hour(), rtc.minute(), rtc.second());
              */
              char filename[16] = {0};
              sprintf(filename, "PICT%03d.JPG", take_picture_count);

              Println("Save taken picture as ");
              Println(filename);

              /* Remove the old file with the same file name as new created file,
                and create new file.
              */

              theSD.remove(filename);
              File myFile = theSD.open(filename, FILE_WRITE);
              myFile.write(img.getImgBuff(), img.getImgSize());
              myFile.close();
            }
            else {
              Println("Failed to take picture");
            }
          }
        }
        theCamera.startStreaming(false, CamCB);
        theCamera.end();
        ScreenReset();
        break;
      /*********************************************************************************************************/
      case '2':
      case 16718055:
        Wire.write("reko-dowo, kaishishimasu.");
        Wire.write(0x0D);
        Wire.endTransmission();    // stop transmitting
        while (results.value != 16753245) {
          IRreceive();
          Record();
        }
        ScreenReset();
        break;
      /*spresense sample sketch player********************************************************************************************************/
      case '3':
      case 16743045:

        ScreenReset();
        break;
      /****************************************************************************************/
      case '4':
      case 16716015:
        ScreenReset();
        Println("LED control");
        while (results.value != 16753245) {
          IRreceive();
          if (results.value == 16748655) {    //up
            light_value = light_value + 5;
          }
          if (results.value == 16769055) {   //down
            light_value = light_value - 5;
          }
          analogWrite(LED, light_value);
          delay(100);
        }

        ScreenReset();
        break;
      /******************************************************************************************/
      case '5':
      case 16726215:
        while (results.value != 16753245) {
          ScreenReset();
          Println("Screen Rotation");
          IRreceive();
          if (digitalRead(BALL_SWITCH) == HIGH) {
            tft.setRotation(3);
          }
          else {
            tft.setRotation(1);
          }
          delay(500);
        }
        ScreenReset();
        break;
      /********************************************************************************************/
      case '6':
      case 16734885:
        tft.fillScreen(ILI9341_RED);
        Wire.beginTransmission(0x2E); // transmit to device 0x2E
        Wire.write("idouwo,/ kaishishimasu.");
        Wire.write(0x0D);
        Wire.endTransmission();
        tft.setTextSize(3);
        Println("Motor");
        Println("Control");
        yield();
        tft.setRotation(3); // ディスプレイの向きを設定
        digitalWrite(MD1_PWM, LOW);
        digitalWrite(MD2_PWM, LOW);
        while (results.value != 16753245) {
          IRreceive();
          if (results.value == 16748655) {
            analogWrite(MD1_PWM, m_power);
            analogWrite(MD2_PWM, m_power);
            digitalWrite(MD1_dir, HIGH);
            digitalWrite(MD2_dir, HIGH);
          }
          else if (results.value == 16769055) {
            analogWrite(MD1_PWM, m_power);
            analogWrite(MD2_PWM, m_power);
            digitalWrite(MD1_dir, LOW);
            digitalWrite(MD2_dir, LOW);
          }
          else if (results.value == 16712445) {
            digitalWrite(MD1_PWM, LOW);
            digitalWrite(MD2_PWM, LOW);
          }
        }
        ScreenReset();
        break;
      /*******************************************************************************************/
      case '7':
      case 16728765:
        break;
      case '8':
      case 16730805:
        break;
      case '9':
      case 16732845:
        break;
      case '0':
      case 16738455:
        Wire.beginTransmission(0x2E); // transmit to device 0x2E
        Wire.write("kurokku, kaisi.");       //send message
        Wire.write(0x0D);
        Wire.endTransmission();    // stop transmitting
        if (Serial.available()) {
          if (Serial.find(TIME_HEADER)) {
            uint32_t pctime = Serial.parseInt();
            RtcTime rtc(pctime);
            RTC.setTime(rtc);
          }
        }
        updateClock();
        break;

      case 'r':
      case 16753245:
        tft.fillScreen(ILI9341_BLACK);
        yield();
        tft.setCursor(0, 0);
        break;

      default:
        if (disp_wait == 0) {
          Println("Waiting...");
          disp_wait++;
        }
    }
    delay(100);
  }
  disp_wait--;
  delay(100);
}

/*****************************************************************************************/






/*
  void SetStatus() {
  int digitalVal = digitalRead(ballsw);
  if(HIGH == ballsw)
  {
    digitalWrite(ledPin,LOW);//turn the led off
  }
  else
  {
    digitalWrite(ledPin,HIGH);//turn the led on
  }
  }
*/

/****************************************************************************/
