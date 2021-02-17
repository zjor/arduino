#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#include <JoystickShieldRF.h>

#define CE_PIN  2
#define CSN_PIN 9

RF24 radio(CE_PIN, CSN_PIN);

const uint64_t pipe = 0xE8E8F0F0E1LL;
const byte address[6] = "00001";

Packet packet;

Servo servo;
int servoAngle = 90;

void log_packet(Packet);

void setup() {
  Serial.begin(115200);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  
  radio.startListening();

  servo.attach(3);
}

void loop() {
  if (radio.available()) {    
    radio.read(&packet, sizeof(packet));
    log_packet(packet);
  }

  if (packet.a == 0) {
    servoAngle++;
  }

  if (packet.b == 0) {
    servoAngle--;
  }

  servo.write(servoAngle);

}

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