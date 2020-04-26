#include <Arduino.h>
#include <SoftwareSerial.h>

#define TX  10
#define RX  11

SoftwareSerial hc06(TX, RX);

long lastTimestamp = 0;

void setup() {
  Serial.begin(9600);
  hc06.begin(9600);
  lastTimestamp = millis();
}

void loop() {
  while (hc06.available() > 0) {
    int c = hc06.read();
    Serial.print((char)c);
  }

  if (millis() - lastTimestamp > 2000) {
    hc06.println("Hello world");
    lastTimestamp = millis();
  }  
  
}