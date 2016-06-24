/**
 * Demonstrates usage of joystick.
 * Uses 8x8 LED display to display pixel.
 * Lets control pixel with a joystick.
 * By pressing joystick button resets pixel coordinates 
 * to the origin.
 */

#include <LedControl.h>

/* Joystick PINs */
#define VRX     A0
#define VRY     A1
#define SW      2

/* Display PINs */
#define CLK 8
#define CS  9
#define DIN 10

LedControl lc = LedControl(DIN, CLK, CS, 1);

volatile int x, y;
int dx, dy;

void setup() {
  pinMode(SW, INPUT_PULLUP);
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  attachInterrupt(digitalPinToInterrupt(SW), resetPixel, RISING);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);

}

void loop() {
  lc.setLed(0, x, y, 0);

  dx = map(analogRead(VRX), 0, 906, 2, -2);
  dy = map(analogRead(VRY), 0, 906, -2, 2);
  if (dx != 0) {dx = dx / abs(dx);}
  if (dy != 0) {dy = dy / abs(dy);}
  x += dx;
  y += dy;

  lc.setLed(0, x, y, 1);
  delay(300);
}

void resetPixel() {
  x = y = 0;
}

