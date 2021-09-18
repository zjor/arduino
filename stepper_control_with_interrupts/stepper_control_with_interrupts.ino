/**
 * Runs stepper interrupt at 200kHz:
 * - compare match register = 16MHz / (prescaler * 200kHz) - 1 => 10, prescaler = 8, 
 * - possible to use 8-bit timers
 * 
 * Runs velocity update at 5kHz:
 * - OCR*A = 16MHz / (prescaler * 1kHz) - 1 = 49, prescaler = 64
 * References: 
 * - https://www.instructables.com/Arduino-Timer-Interrupts/
 * - https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 */

#define DIR_PIN   5
#define STEP_PIN  6

#define PPR   1600
#define TICKS_PER_SECOND  200000 // 200kHz
#define PULSE_WIDTH 1

void setTimer1(int ocra) {  
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0  
  OCR1A = ocra;
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS11);  // set prescaler to 8  
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
}

void setTimer2(int ocra) {
  TCCR2A = 0; // set entire TCCR1A register to 0
  TCCR2B = 0; // same for TCCR1B
  TCNT2  = 0; // initialize counter value to 0  
  OCR2A = ocra;
  TCCR2A |= (1 << WGM21); // turn on CTC mode
  TCCR2B |= (1 << CS22);  // set prescaler to 64  
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
}


void setTimers() {
  cli();
  setTimer1(9); // 200kHz
  setTimer2(49); //5kHz
  sei();
}

volatile unsigned long currentTick = 0UL;
volatile unsigned long ticksPerPulse = UINT64_MAX;
volatile float accel = 0.0;
volatile float velocity = 0.0;

const float accelStep = 10.0;
const float maxVelocity = 20.0;

void setup() {
  Serial.begin(115200);
  setTimers();
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
  accel = accelStep;
}

void loop() {
  if (velocity > maxVelocity) {
    accel = -accelStep;
  }

  if (velocity < -maxVelocity) {
    accel = accelStep;
  }
  Serial.println(ticksPerPulse);
}

/**
 * Stepper control interrupt handler
 */
ISR(TIMER1_COMPA_vect) {
  if (currentTick >= ticksPerPulse) {
    currentTick = 0;
  }

  if (currentTick == 0) {
    digitalWrite(STEP_PIN, HIGH);
  } else if (currentTick == PULSE_WIDTH) {
    digitalWrite(STEP_PIN, LOW);
  }
  
  currentTick++;
}

/**
 * Velocity update interrupt handler
 */
ISR(TIMER2_COMPA_vect) {
  velocity += accel * 2e-4;
  if (abs(velocity) < 1e-3) {
    ticksPerPulse = UINT64_MAX;
  } else {
    ticksPerPulse = (uint64_t)(2.0 * PI * TICKS_PER_SECOND / (abs(velocity) * PPR)) - PULSE_WIDTH;
  }
  digitalWrite(DIR_PIN, velocity > 0 ? HIGH : LOW);
}
