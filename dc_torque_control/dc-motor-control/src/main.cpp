#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define OUTPUT_A PD3
#define OUTPUT_B PD2

#define PWM_PIN 9
#define DIR_PIN 4

#define PPR 5000

int motor_dir = HIGH;

volatile long encoderValue = 0L;
volatile long lastEncoded = 0L;

void encoderHandler();

Adafruit_INA219 ina219;

void log_state() {
  float amps = ina219.getCurrent_mA();
  Serial.print(amps, 2);
  Serial.print("\t");
  Serial.println(2 * 3.14 * encoderValue / PPR);
}

void set_pwm_frequency() {
//  TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//  TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz  
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
}

void setup() {
  pinMode(OUTPUT_A, INPUT_PULLUP);
  pinMode(OUTPUT_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), encoderHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(OUTPUT_B), encoderHandler, CHANGE);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  set_pwm_frequency();
  ina219.begin();
  Serial.begin(115200);  
}

int log_i = 0;

void loop() {

  analogWrite(PWM_PIN, 128);
  if (encoderValue >= PPR) {
    motor_dir = LOW;
  }
  if (encoderValue <= -PPR) {
    motor_dir = HIGH;
  }

  digitalWrite(DIR_PIN, motor_dir);
  
  log_i++;
  if (log_i > 5000) {
    log_state();
    log_i = 0;
  }
  
}


void encoderHandler() {
  int MSB = (PIND & (1 << OUTPUT_A)) >> OUTPUT_A; //MSB = most significant bit
  int LSB = (PIND & (1 << OUTPUT_B)) >> OUTPUT_B; //LSB = least significant bit
  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderValue++; //CW
  }
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderValue--; //CCW
  }

  lastEncoded = encoded; //store this value for next time  
}