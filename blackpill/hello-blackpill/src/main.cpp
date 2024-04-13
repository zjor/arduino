#include <math.h>
#include <Arduino.h>

#define N 64

uint16_t state = HIGH;
uint32_t delays[N] = {};

void setup() {
  pinMode(PC13, OUTPUT);
  for (int i = 0; i < N; i++) {
    delays[i] = (int)(200.0 + 150.0 * sin(2.0 * i * PI / N));
  }
}

uint16_t i = 0;

void loop() {
  digitalWrite(PC13, state);
  state = !state;
  
  delay(delays[i]);
  i = (++i) % N;
}