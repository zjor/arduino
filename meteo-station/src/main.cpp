/**

## Connections ##
LED I2C library: https://github.com/marcoschwartz/LiquidCrystal_I2C
A4 (SDA) -> LCD SDA
A5 (SCL) -> LCD SCL
*/

#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#include <dht11.h>

#include "SlidingWindow.h"

#define MH_Z19_RX 9
#define MH_Z19_TX 8

#define DHT11_PIN 7

SoftwareSerial co2Serial(MH_Z19_RX, MH_Z19_TX);
LiquidCrystal_I2C lcd(0x27, 16, 2);
dht11 dht11;

SlidingWindow temperatureAvg(5);

int readCO2();
void printTemperature(int);
void printHumidity(int);
void printCO2(int);

void setup() {
  co2Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  printTemperature(15);
  printHumidity(34);
  printCO2(450);
}

void loop() {
  dht11.read(DHT11_PIN);
  printTemperature((int)temperatureAvg.add(dht11.temperature));
  printHumidity(dht11.humidity);
  printCO2(readCO2());
  delay(3000);
}

int readCO2() {

  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

  byte response[9];

  co2Serial.write(cmd, 9);
  co2Serial.readBytes(response, 9);

  if (response[0] != 0xFF) {
    return -1;
  }

  if (response[1] != 0x86) {
    return -1;
  }

  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256 * responseHigh) + responseLow;
  return ppm;
}

void printTemperature(int t) {
  char buffer[16];
  sprintf(buffer, "T=%d%cC%c", t, 223, '\0');
  lcd.setCursor(0, 0);
  lcd.print(buffer);
}

void printHumidity(int h) {
  char buffer[16];
  sprintf(buffer, "H=%d%%%c", h, '\0');
  lcd.setCursor(8, 0);
  lcd.print(buffer);
}

void printCO2(int co2) {
  char buffer[16];
  sprintf(buffer, "CO2=%dppm%c", co2, '\0');
  lcd.setCursor(0, 1);
  lcd.print(buffer);
}
