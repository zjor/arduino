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
float last_theta = 0;
float theta_u = 0;
float omega = 0;
float omega_u = 0;
Adafruit_INA219 ina219;

#define CURRENT_BUF_SIZE  2
float current_buf[CURRENT_BUF_SIZE];
AvgValue current_ma(current_buf, CURRENT_BUF_SIZE);
float current_avg;
float u;

PID current_pid(0.17, 0.0, 0.75, .0);
PID velocity_pid(40.0, 1.2, 50.0, .0);
PID position_pid(35.0, 3.0, 20.0, 1.5 * M_PI);

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

void run(unsigned long now, unsigned long dt_millis) {
  float dt = 1e-3 * dt_millis;
  
  float theta = M_PI * encoder.read() / PPR;
  omega = (theta - last_theta) / dt;
  last_theta = theta;

  theta_u = position_pid.getControl(theta, dt);
  velocity_pid.setTarget(theta_u);

  omega_u = -velocity_pid.getControl(omega, dt);
  current_pid.setTarget(omega_u);

  current_avg = current_ma.get_avg();

  u = current_pid.getControl(current_avg, dt);

  digitalWrite(DIR_PIN, u > 0 ? HIGH : LOW);
  analogWrite(PWM_PIN, (int) min(abs(u), 255));

  // log_state();  
}

TimedTask run_task(&run, 1);

void loop() {
  run_task.loop();
  current_ma.update(ina219.getCurrent_mA());  
}

int log_i = 0;

void log_state() {
  log_i++;
  if (log_i > 10) {
    Serial.print(last_theta);
    Serial.print("\t");
    Serial.print(theta_u);
    Serial.print("\t");
    Serial.print(omega);
    Serial.print("\t");
    Serial.print(omega_u);
    Serial.print("\t");
    Serial.print(current_avg);
    Serial.print("\t");
    Serial.println(u);
    log_i = 0;
  }
}