#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

struct t_command  {
  int index;
  int value;
};

char buf[64];
int i = 0;

void setup() {
  Serial.begin(9600);
  pwm.begin();  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  yield();
}

void loop() {  
  
  if (Serial.available() > 0) {
    int recv = Serial.read();
    if (recv != '$') {
      buf[i++] = (char) recv;
    } else {
      moveServo(parseBuffer(buf));
      memset(buf, 0, sizeof(buf));
      i = 0;
    }
  }

}

struct t_command parseBuffer(char* buf) {
  Serial.print("Raw buffer: ");
  Serial.println(buf);  
  String str(buf);
  int index = String(str[0]).toInt();
  int value = str.substring(str.indexOf(':') + 1).toInt();
  Serial.print("Index: "); Serial.print(index);
  Serial.print("; Value: ");Serial.println(value);
  return {index, value};
}

void moveServo(struct t_command command) {
  pwm.setPWM(command.index, 0, map(command.value, 0, 180, SERVOMIN, SERVOMAX));
}

