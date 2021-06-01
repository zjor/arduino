/**
 * Runs stepper interrupt at 20kHz:
 * - compare match register = 16MHz / (prescaler * 20kHz) - 1 => 99, prescaler = 8, 
 * - possible to use 8-bit timers
 * 
 * Runs velocity update at 1kHz:
 * - OCR*A = 16MHz / (prescaler * 1kHz) - 1 = 249, prescaler = 64
 * References: 
 * - https://www.instructables.com/Arduino-Timer-Interrupts/
 * - https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 */

void setTimer1At20kHz() {
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0  
  OCR1A = 99; // set compare match register for 1hz increments: (16e6) / (20e3 * 8) - 1  
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS11);  // set prescaler to 8  
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
}

void setTimer2At1kHz() {
  TCCR2A = 0; // set entire TCCR1A register to 0
  TCCR2B = 0; // same for TCCR1B
  TCNT2  = 0; // initialize counter value to 0  
  OCR2A = 249; // set compare match register for 1hz increments: (16e6) / (1e3 * 64) - 1  
  TCCR2A |= (1 << WGM21); // turn on CTC mode
  TCCR2B |= (1 << CS22);  // set prescaler to 64  
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
}


void setTimers() {
  cli();
  setTimer1At20kHz();
  setTimer2At1kHz();
  sei();
}

volatile uint64_t t1 = 0;
volatile uint64_t t2 = 0;

void setup() {
  Serial.begin(115200);
  setTimers();  
}

void loop() {
  if (t1 >= 20e3) {
    t1 = 0;
    Serial.println("Timer1 counted to 20k");
  }

  if (t2 >= 1000) {
    t2 = 0;
    Serial.println("Timer2 couned to 1000");
  }
}

ISR(TIMER1_COMPA_vect) {
  t1++;
}

ISR(TIMER2_COMPA_vect) {
  t2++;
}
