#include <SPI.h>
#include <Arduino.h>
#include <LEDMatrixDriver.hpp>

#define SEGMENTS_COUNT  1
#define CS_PIN          9

LEDMatrixDriver lmd(SEGMENTS_COUNT, CS_PIN);

void setup() {
  lmd.setEnabled(true);
  lmd.setIntensity(2);
  lmd.setPixel(1, 1, true);
  lmd.display();
}

void loop() {
  // put your main code here, to run repeatedly:
}