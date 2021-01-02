#include <Arduino.h>

#define OUTPUT_A PD3
#define OUTPUT_B PD2

#define ADC_PIN 0

#define PWM_PIN 9
#define DIR_PIN 4

#define BUF_SIZE  3
#define PPR 5000

volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;
int value;
int buf[BUF_SIZE];
int buf_pos = 0;

int motor_dir = HIGH;

volatile long encoderValue = 0L;
volatile long lastEncoded = 0L;

void encoderHandler();

float avg(int *a, int len) {
  float sum = 0;
  for (int i = 0; i < len; i++) {
    sum += a[i];
  }
  return sum / len;
}

void log_state() {
  // current reading
  Serial.print(avg(buf, BUF_SIZE) - 512);
  Serial.print("\t");
  // Serial.println(encoderValue);
  Serial.println(0);
}

void adc_init() {
//  TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//  TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
  
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
  ADCSRA =  bit (ADEN);                     
  ADCSRA |= bit (ADPS2);
  ADMUX  =  bit (REFS0) | (ADC_PIN & 0x07);
}

void setup() {
  pinMode(OUTPUT_A, INPUT_PULLUP);
  pinMode(OUTPUT_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), encoderHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(OUTPUT_B), encoderHandler, CHANGE);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  adc_init();

  Serial.begin(9600);  
}

int log_i = 0;

void loop() {
  if (adcDone) {
    adcStarted = false;
    value = adcReading;
    buf[buf_pos] = value;
    buf_pos = (buf_pos + 1) % BUF_SIZE;
    adcDone = false;
  }
    
  if (!adcStarted) {
    adcStarted = true;
    ADCSRA |= bit (ADSC) | bit (ADIE);
  }

  // analogWrite(PWM_PIN, 128);
  if (encoderValue >= PPR) {
    motor_dir = LOW;
  }
  if (encoderValue <= -PPR) {
    motor_dir = HIGH;
  }

  digitalWrite(DIR_PIN, motor_dir);
  
  log_i++;
  if (log_i > 500) {
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

ISR (ADC_vect) {
  adcReading = ADC;
  adcDone = true;  
}