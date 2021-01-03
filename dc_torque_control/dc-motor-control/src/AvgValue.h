#ifndef AVG_VALUE_H
#define AVG_VALUE_H


class AvgValue {
  public:
    AvgValue(float*, unsigned int);
    void update(float);
    float get_avg();
  private:
    unsigned int _size;
    unsigned int _i;
    float *_buf;
};

AvgValue::AvgValue(float* buf, unsigned int size) {
  _buf = buf;
  _size = size;
  _i = 0;
}

void AvgValue::update(float val) {
  _buf[_i] = val;
  _i++;
  if (_i >= _size) {
    _i = 0;
  }
}

float AvgValue::get_avg() {
  float sum = 0;
  for (int i = 0; i < _size; i++) {
    sum += _buf[i];
  }
  return sum / _size;
}

#endif