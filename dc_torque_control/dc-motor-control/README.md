# DC motor control

## Log

INA219 current reading with Arduino UNO takes ~1008µs or ~1ms

## Tasks

- develop arduino time-bound routine, executing approx at the same rate, passing dt as an argument
- control current with PI regulator
- read absolute angular velocity
- control velocity with a double feedback loop
- control motor position with a tripple feedback loop
- use DC775