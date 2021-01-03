#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include "PID.h"
#include "TimedTask.h"
#include "AvgValue.h"

#define PWM_PIN 9
#define DIR_PIN 4

#define MAX_VOLTAGE 5.51

#define PPR 5000

Encoder encoder(3, 2);
Adafruit_INA219 ina219;

float current_buf[3];
AvgValue current_ma(current_buf, 3);
float current_avg;
float u;

PID current_pid(0.5, 0.0, 1e-3, 75.0);

void log_state();

void set_pwm_frequency() {
//  TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//  TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz  
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
}

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  set_pwm_frequency();

  ina219.begin();
  Serial.begin(115200);
}

void run(unsigned long now, unsigned long dt) {
  current_avg = current_ma.get_avg();

  u = current_pid.getControl(current_avg, 1.0 * dt * 1e-3);

  digitalWrite(DIR_PIN, u < 0 ? HIGH : LOW);
  analogWrite(PWM_PIN, min(abs(u), 255));

  log_state();  
}

TimedTask run_task(&run, 20);

void loop() {
  run_task.loop();
  current_ma.update(ina219.getCurrent_mA());  
}

int log_i = 0;

void log_state() {
  log_i++;
  if (log_i > 10) {
    Serial.print(current_avg);
    Serial.print("\t");
    Serial.println(u);
    log_i = 0;
  }
}