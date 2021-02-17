#include <Arduino.h>

/**
 * Joystick Shield sketch
 * 
 * source: https://drive.google.com/file/d/17bpZXWGHnV3cDMLiMUFQdgDcT58kwSu8/view?usp=sharing
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <JoystickShieldRF.h>

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
Packet packet;

void log_packet(Packet p) {
  Serial.print(*(uint32_t *) &packet);
  Serial.print(": ");
  Serial.print(p.a);Serial.print(' ');
  Serial.print(p.b);Serial.print(' ');
  Serial.print(p.c);Serial.print(' ');
  Serial.print(p.d);Serial.print(' ');
  Serial.print(p.e);Serial.print(' ');
  Serial.print(p.f);Serial.print(' ');
  Serial.print(p.down);Serial.print(' ');
  Serial.print(p.x);Serial.print(' ');
  Serial.println(p.y);
}


void setup() {

  Serial.begin(115200);

  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP); 
  pinMode(KEY_C, INPUT_PULLUP); 
  pinMode(KEY_D, INPUT_PULLUP);
  pinMode(KEY_E, INPUT);
  pinMode(KEY_F, INPUT);
  pinMode(KEY_DOWN, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}

void loop() {
  packet.a = digitalRead(KEY_A);
  packet.b = digitalRead(KEY_B);
  packet.c = digitalRead(KEY_C);
  packet.d = digitalRead(KEY_D);
  packet.e = digitalRead(KEY_E);
  packet.f = digitalRead(KEY_F);
  packet.down = digitalRead(KEY_DOWN);
  packet.x = analogRead(JOYSTICK_X);
  packet.y = analogRead(JOYSTICK_Y);
  
  log_packet(packet);

  delay(50);

  radio.write(&packet, sizeof(packet));
}
