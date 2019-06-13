#include <Arduino.h>

#include "Timer.h"

Timer::Timer(unsigned long interval, timer_callback callback) {
  this->interval = interval;
  this->callback = callback;  
}

void Timer::run() {
  unsigned long now = millis();
  if (now - lastRunTimestamp >= interval) {
    callback(now - lastRunTimestamp);
    lastRunTimestamp = now;
  }      
}
