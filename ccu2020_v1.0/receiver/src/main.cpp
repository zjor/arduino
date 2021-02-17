#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <JoystickShieldRF.h>

#define CE_PIN  2
#define CSN_PIN 9

RF24 radio(CE_PIN, CSN_PIN);

const uint64_t pipe = 0xE8E8F0F0E1LL;
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  
  radio.startListening();

  Serial.println(shield);

  
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    for (int i = 0; i < 32; i++) {
      Serial.print(text[i], DEC);
      Serial.print(" ");      
    }
    Serial.println();
  }
}