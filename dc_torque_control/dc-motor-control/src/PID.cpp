#include "PID.h"

float clamp(float value, float min, float max);

PID::PID(float Kp, float Kd, float Ki, float target) {
    _Kp = Kp;
    _Kd = Kd;
    _Ki = Ki;
    _target = target;
}

PID::PID(float Kp, float Kd, float Ki, float target, float min_u, float max_u) {
    _Kp = Kp;
    _Kd = Kd;
    _Ki = Ki;
    _target = target;
    _min_u = min_u;
    _max_u = max_u;
}

float PID::getControl(float value, float dt_seconds) {
    float lastValue = _has_last_value ? _last_value : value;
    float error = _target - value;
    float de = -(value - lastValue) / dt_seconds;
    _integralError += _Ki * error * dt_seconds;
    _lastError = error;
    _last_value = value;
    _has_last_value = true;
    return clamp((_Kp * error + _Kd * de + _integralError), _min_u, _max_u);
}

void PID::setTarget(float target) {
    _target = target;
}

float clamp(float value, float min, float max) {
  if (value > max) {
    return max;
  }
  if (value < min) {
    return min;
  }
  return value;
}