#include "Timer.h"

#define DIR_A 12
#define PWM_A 3

#define OUTPUT_A  4
#define OUTPUT_B  2

// pulses per revolution
#define PPR   240

#define Kp  60.0
#define Kd  1.0
#define Ki  5.0

volatile long encoderPosition = 0;
long lastEncoderPosition = 0;

// reference angle
float ref_x = 1024;

// measured angle
float est_x = .0;
float last_x = .0;

float velocity = .0;

float error_acc = .0;
float last_error = .0;

int dir = HIGH;
float pwm = 0.0;

void measure_callback(unsigned long);
void control_callback(unsigned long);
void logger_callback(unsigned long);

Timer measurer(32, &measure_callback);
Timer controller(32, &control_callback);
Timer logger(100, &logger_callback);

void setup() {
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);

  pinMode(OUTPUT_A, INPUT_PULLUP);
  pinMode(OUTPUT_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), aHandler, RISING); 
  attachInterrupt(digitalPinToInterrupt(OUTPUT_B), bHandler, RISING);  

  Serial.begin(115200);
  
  analogWrite(PWM_A, int(pwm));
  digitalWrite(DIR_A, dir);
}

void loop() {
  if (millis() > 25 * 1000) {
    analogWrite(PWM_A, 0);
    return;    
  }

  measurer.run();
  logger.run();
  controller.run();
  
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

void measure_callback(unsigned long dt) {
  est_x = encoderPosition;
  
  velocity = 2.0 * PI * 1000.0 * (est_x - last_x) / PPR / dt;
  last_x = est_x; 
}

void control_callback(unsigned long dt) {
  float error = 1.0 * (ref_x - est_x) / PPR;
  error_acc += error * dt / 1000.0;
  float d_error = 1000.0 * (error - last_error) / dt;
  d_error = - velocity;
  last_error = error;
  float u = Kp * error + Kd * d_error + Ki * error_acc;

  pwm = u;
  if (pwm > 255) {
    pwm = 255;
  } else if (pwm < -255) {
    pwm = -255;
  }

  analogWrite(PWM_A, abs(int(pwm)));
  
  if (pwm > 0) {
    digitalWrite(DIR_A, HIGH);
  } else {
    digitalWrite(DIR_A, LOW);
  }
  
}

void logger_callback(unsigned long dt) {
  Serial.print(ref_x);
  Serial.print("\t");
  Serial.print(est_x);
  Serial.print("\t");
  Serial.print(pwm);
  Serial.print("\t");
  Serial.println(velocity);    
}
