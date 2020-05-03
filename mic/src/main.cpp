#include <Arduino.h>

volatile uint16_t sample;

void initADC() {
  // Init ADC; f = ( 16MHz/prescaler ) / 13 cycles/conversion 
  ADMUX  = 0; // Channel sel, right-adj, use AREF pin
  ADCSRA = _BV(ADEN)  | // ADC enable
           _BV(ADSC)  | // ADC start
           _BV(ADATE) | // Auto trigger
           _BV(ADIE)  | // Interrupt enable
           _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // 128:1 / 13 = 9615 Hz
  ADCSRB = 0; // Free-run mode
  DIDR0  = _BV(0); // Turn off digital input for ADC pin      
  TIMSK0 = 0;                // Timer0 off
}

void setup() {
  cli();
  initADC();
  sei();

  Serial.begin(9600);  
}

void loop() {  
  Serial.println(sample);
  delay(250);
}

ISR(ADC_vect) { 
  sample = ADC;
}