#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#include <JoystickShieldRF.h>

// nRF pins
#define CE_PIN  2
#define CSN_PIN 9

// motor A pins
#define AIN1  10
#define AIN2  A3
#define APWM  5

// motor B pins
#define BIN1  A5
#define BIN2  A4
#define BPWM  6


RF24 radio(CE_PIN, CSN_PIN);

const uint64_t pipe = 0xE8E8F0F0E1LL;
const byte address[6] = "00001";

Packet packet;

// Servo servoX;
// Servo servoY;

void log_packet(Packet);

void setupMotors() { 
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(APWM, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(BPWM, OUTPUT);
}

void driveMotors(Packet);

void setup() {
  Serial.begin(115200);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  
  radio.startListening();

  setupMotors();

  // servoX.attach(3);
  // servoY.attach(4);
}

void loop() {
  if (radio.available()) {    
    radio.read(&packet, sizeof(packet));
    log_packet(packet);
    driveMotors(packet);
    // servoX.write(map(packet.x, 0, 1023, 0, 180));
    // servoY.write(map(packet.y, 0, 1023, 0, 180));
  }
}

void driveMotors(Packet p) {
  float vx = p.x - 512;
  float vy = p.y - 512;
  float vl = (1.0 + vx / 512 / 2) * vy;
  float vr = (1.0 - vx / 512 / 2) * vy;

  int dir = vl > 0;
  digitalWrite(AIN1, !dir);
  digitalWrite(AIN2, dir);
  analogWrite(APWM, map(abs(vl), 0, 512, 0, 255));

  dir = vr > 0;
  digitalWrite(BIN1, dir);
  digitalWrite(BIN2, !dir);
  analogWrite(BPWM, map(abs(vr), 0, 512, 0, 255));
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