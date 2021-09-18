#include <Arduino.h>
#include <SPI.h>

volatile byte c;
volatile boolean dataReady = false;
int pos = 0;
char buf[100];

void setup() {
    Serial.begin (115200);

    // turn on SPI in slave mode
    SPCR |= bit (SPE);
    pinMode(MISO, OUTPUT);

    SPI.attachInterrupt();
}

void loop() {
    if (dataReady) {
        buf[pos++] = c;
        dataReady = false;
        if (c == '\n') {
            buf[pos] = '\0';
            pos = 0;
            Serial.println(buf);
        }
    }
}

ISR (SPI_STC_vect) {
    c = SPDR;
    dataReady = true;
}