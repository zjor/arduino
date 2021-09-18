#include <Arduino.h>
#include <SPI.h>

#define SS_1    8
#define SS_2    7

void setup() {
    pinMode(SS_1, OUTPUT);
    pinMode(SS_2, OUTPUT);
    digitalWrite(SS_1, HIGH);
    digitalWrite(SS_2, HIGH);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void transferMessage(int ss) {
    digitalWrite(ss, LOW);
    char c;
    for (const char * p = "Hello, world!\n" ; c = *p; p++) {
        SPI.transfer(c);
    }
    digitalWrite(ss, HIGH);
}

void transferLedState(int ss, int state) {
    digitalWrite(ss, LOW);
    SPI.transfer(state);
    digitalWrite(ss, HIGH);
}

void loop() {
    static int ledState = LOW;
    transferLedState(SS_1, ledState);
    ledState = !ledState;

    transferMessage(SS_2);
    delay(250);
}