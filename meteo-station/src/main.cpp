/**

## Connections ##
LED I2C library: https://github.com/marcoschwartz/LiquidCrystal_I2C
A4 (SDA) -> LCD SDA
A5 (SCL) -> LCD SCL
*/

#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <dht11.h>

#define DHT11_PIN 7

LiquidCrystal_I2C lcd(0x27, 16, 2);
dht11 dht11;

void printTemperature(int);
void printHumidity(int);
void printCO2(int);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  printTemperature(15);
  printHumidity(34);
  printCO2(450);
}

void loop() {
  int chk = dht11.read(DHT11_PIN);
  printTemperature(dht11.temperature);
  printHumidity(dht11.humidity);
  delay(3000);
}

void printTemperature(int t) {
  char buffer[16];
  sprintf(buffer, "T=%d%cC", t, 223);
  lcd.setCursor(0, 0);
  lcd.print(buffer);
}

void printHumidity(int h) {
  char buffer[16];
  sprintf(buffer, "H=%d%%", h);
  lcd.setCursor(8, 0);
  lcd.print(buffer);
}

void printCO2(int co2) {
  char buffer[16];
  sprintf(buffer, "CO2=%dppm", co2);
  lcd.setCursor(0, 1);
  lcd.print(buffer);
}
