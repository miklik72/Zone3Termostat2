

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SensorT25.h>
#include <RTClib.h>
#include <ESPRotary.h>
#include "Button2.h"

// LDC pins
#define TFT_DC 13
#define TFT_CS 5
#define TFT_RST 19
// RF 433MHz pins
#define IRQ_PIN 12           // RF input with irq
//RTC
#define RTC_SDA 21
#define RTC_SLC 22
//Encoder
#define ENC_A 27
#define ENC_B 26
#define ENC_C 14            //click

//LCD
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//RTC
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Nedele", "Pondeli", "Utery", "Streda", "Ctvrtek", "Patek", "Sobota"};
//Encoder
ESPRotary enc = ESPRotary(ENC_A, ENC_B);
Button2 enc_btn = Button2(ENC_C);
long enc_position  = -999;

uint16_t hmax, wmax;
long oldt;

void rotate(ESPRotary& enc) {
   tft.setCursor(20, 160);
   tft.setTextSize(2);
   tft.println(enc.getPosition());
}

void showDirection(ESPRotary& enc) {
  tft.setCursor(60, 160);
  tft.setTextSize(2);
  tft.println(enc.directionToString(enc.getDirection()));
}

void showPosition(Button2& enc_btn) {
  tft.setCursor(20, 160);
  tft.setTextSize(2);
  tft.println(enc.getPosition());
}

void resetPosition(Button2& enc_btn) {
  enc.resetPosition();
  tft.setCursor(20, 160);
  tft.setTextSize(2);
  tft.print(enc.getPosition());
  tft.println(" Reset!");
}

void setup(void) {
  delay(500);
  Serial.begin(115200);
  SensorT25::enable(IRQ_PIN);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date &amp; time this sketch was compiled
    rtc.adjust(DateTime(2018,8,23,21,50,0));
    // This line sets the RTC with an explicit date &amp; time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  tft.begin();
  tft.setRotation(3);
  wmax = tft.width();
  hmax = tft.height();
  tft.setCursor(0, 0);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.print("W");
  tft.print(wmax);
  tft.print("xH");
  tft.println(hmax);

  enc.setChangedHandler(rotate);
  enc.setLeftRotationHandler(showDirection);
  enc.setRightRotationHandler(showDirection);
  enc_btn.setClickHandler(showPosition);
  enc_btn.setLongClickHandler(resetPosition);
}

void loop(void) {
  DateTime now = rtc.now();
  long delta = 1000;
  long t = millis();
  enc.loop();
  enc_btn.loop();
  if(oldt + delta < t) {
    tft.setCursor(0, 20);
    tft.setTextSize(2);
    for(int i = 0; i < SENSCOUNT; i++) {
      Serial.print("CH ");
      Serial.print(i+1);                //channel
      Serial.print("-V ");
      Serial.print(SensorT25::isValid(i));         //valid data
      Serial.print("-SID ");
      Serial.print(SensorT25::getSID(i));          //sensor ID
      Serial.print("-TEMP ");
      Serial.print(SensorT25::getTemperature(i),1);
      Serial.print("-AGE ");
      Serial.print(SensorT25::getValueAge(i));
      Serial.println();
      //LCD
      tft.print("CH");
      tft.print(i+1);                //channel
      tft.print(" V");
      tft.print(SensorT25::isValid(i));         //valid data
      tft.print(" ID");
      tft.print(SensorT25::getSID(i));          //sensor ID
      tft.print(" T ");
      tft.print(SensorT25::getTemperature(i),1);
      tft.setTextSize(1);
      tft.print(" AGE ");
      tft.print(SensorT25::getValueAge(i));
      tft.setTextSize(2);
      tft.println();
      tft.println();
    }
    oldt = t;
    tft.setCursor(0, 120);
    tft.print(now.year(), DEC);
    tft.print('/');
    tft.print(now.month(), DEC);
    tft.print('/');
    tft.print(now.day(), DEC);
    tft.print(" (");
    tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
    tft.println(")");
    tft.print(now.hour(), DEC);
    tft.print(':');
    tft.print(now.minute(), DEC);
    tft.print(':');
    tft.print(now.second(), DEC);
    tft.println();
  }
  //delay(1000);
}
