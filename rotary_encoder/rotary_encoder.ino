/**
 * This is an example of usage of an incremental optical rotary encoder.
 * Encoder model: LPD3806-600BM-G5-24C
 * Wiring:
 *  - Green - A phase
 *  - White - B phase
 *  - Red   - 5-24V Vcc
 *  - Black - ground
 */
#include <AccelStepper.h>

#define OUTPUT_A  2
#define OUTPUT_B  3

#define DIR_PIN   9
#define STEP_PIN  8

volatile int encoderPosition = 0;
int lastValue = 0;

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
  pinMode(OUTPUT_A, INPUT_PULLUP);
  pinMode(OUTPUT_B, INPUT_PULLUP);

  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), aHandler, RISING); 
  attachInterrupt(digitalPinToInterrupt(OUTPUT_B), bHandler, RISING);

  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(2000);

  Serial.begin(115200);
}

void loop() {
  if (lastValue != encoderPosition) {
    lastValue = encoderPosition;
    Serial.println(encoderPosition);
    stepper.moveTo(encoderPosition / 2);
  }

  stepper.run();
  
}

void aHandler() {
  if (digitalRead(OUTPUT_B) == LOW) {
    encoderPosition++;
  } else {
    encoderPosition--;
  }
}

void bHandler() {
  if (digitalRead(OUTPUT_A) == LOW) {
    encoderPosition--;
  } else {
    encoderPosition++;
  }
}
