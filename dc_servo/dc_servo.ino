/**
 * JGA25-370 DC12V280RPM
 * Gear ration: 21.3
 * TODO:
 * - change PWM frequency
 * - decrease reponse time
 * - check PID set target
 */
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include "PID.h"
#include "TimedTask.h"

#define ENC_A  18
#define ENC_B  19

#define EN1_PIN 7
#define EN2_PIN 6
#define PWM_PIN 5

#define PPR (11 * 21.3 * 2)

Encoder encoder(ENC_A, ENC_B);

PID velocity_pid(35.0, -0.5, 35.0, 0.0);
PID position_pid(10.0, -0.1, 5.0, 0.0);
float u;
float w;
float theta;
float last_theta = 0;
float pos_target = .0;

int i_log = 0;
void log_state() {
  i_log++;
  if (i_log > 10) {
    Serial.print(u / 12);
    Serial.print("\t");
    Serial.print(w);
    Serial.print("\t");
    Serial.print(theta);
    Serial.print("\t");
    Serial.println(pos_target);    
    i_log = 0;
  }
}

void run(unsigned long now, unsigned long dt_millis) {
  float dt = 1e-3 * dt_millis;

  int enc_value = encoder.read();
  theta = M_PI * enc_value / PPR;
  w = 1.0 * (theta - last_theta) / dt;
  last_theta = theta;

  float theta_u = position_pid.getControl(theta, dt);
  velocity_pid.setTarget(theta_u);

  u = velocity_pid.getControl(w, dt);
  
  if (u > 0) {
    digitalWrite(EN1_PIN, LOW);
    digitalWrite(EN2_PIN, HIGH);
  } else {
    digitalWrite(EN2_PIN, LOW);
    digitalWrite(EN1_PIN, HIGH);
  }
  analogWrite(PWM_PIN, (int) min(abs(u), 255));
  log_state();

  int pos = analogRead(A10);
  pos_target = M_PI * (1.0 * pos / 512.0 - 1.0);
  position_pid.setTarget(pos_target);
}

TimedTask run_task(&run, 20);

void setup() {   
  pinMode(EN1_PIN, OUTPUT);
  pinMode(EN2_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  
  Serial.begin(115200);

  digitalWrite(EN1_PIN, HIGH);
  digitalWrite(EN2_PIN, LOW);
  analogWrite(PWM_PIN, 63);
}

void loop() {
  run_task.loop();
}
