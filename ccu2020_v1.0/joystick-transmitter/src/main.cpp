#include <Arduino.h>

/**
 * Joystick Shield sketch
 * 
 * source: https://drive.google.com/file/d/17bpZXWGHnV3cDMLiMUFQdgDcT58kwSu8/view?usp=sharing
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
#define JOYSTICK_X   A1
#define JOYSTICK_Y   A0
#define KEY_A 2
#define KEY_B 3
#define KEY_C 4
#define KEY_D 5
#define KEY_E 6
#define KEY_F 7
#define KEY_DOWN 8

const uint64_t pipe = 0xE8E8F0F0E1LL;

const byte address[6] = "00001";

RF24 radio(CE_PIN, CSN_PIN);
int packet[9];

void setup() {

  Serial.begin(115200);

  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP); 
  pinMode(KEY_C, INPUT_PULLUP); 
  pinMode(KEY_D, INPUT_PULLUP);
  pinMode(KEY_E, INPUT);
  pinMode(KEY_F, INPUT);
  pinMode(KEY_DOWN, INPUT);

  // radio.begin();
  // radio.setAutoAck(0);
  // radio.setRetries(0, 15);
  // radio.enableAckPayload();
  // radio.setPayloadSize(32);
  // radio.openWritingPipe(pipe);
  // radio.setChannel(9);
  // radio.setPALevel (RF24_PA_MAX);   
  // radio.setDataRate (RF24_250KBPS);  
  // radio.powerUp();
  // radio.stopListening();

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}

void loop() {
  packet[0] = digitalRead(KEY_DOWN);
  packet[1] = analogRead(JOYSTICK_X); 
  packet[3] = analogRead(JOYSTICK_Y); 
  packet[2] = digitalRead(KEY_E); 
  packet[4] = digitalRead(KEY_F); 
  packet[5] = digitalRead(KEY_D); 
  packet[6] = digitalRead(KEY_B); 
  packet[7] = digitalRead(KEY_A); 
  packet[8] = digitalRead(KEY_C); 

  for (int i = 0; i < 9; i++) {
    Serial.print(packet[i]);
    Serial.print(" ");
  }
  Serial.println();

  delay(50);

  //TODO: send only if state has changed
  radio.write(packet, sizeof(packet));
}
