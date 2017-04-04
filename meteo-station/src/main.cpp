/**

## Connections ##
LED I2C library: https://github.com/marcoschwartz/LiquidCrystal_I2C
A4 (SDA) -> LCD SDA
A5 (SCL) -> LCD SCL
*/

#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  lcd.setCursor(pos, 0);
  lcd.print(line);

  lcd.setCursor(pos2, 1);
  lcd.print(line2);
}

void printTemperature(int t) {

}
