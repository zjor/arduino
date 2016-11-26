#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

#define LED_PIN   13

struct t_command  {
  int index;
  int value;
};

char buf[64];
int i = 0;

int pos[3] = {90,90,90};
int target[3] = {90,90,90};
int nop[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  pwm.begin();  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  pinMode(LED_PIN, OUTPUT);
  yield();
}

void loop() {  
  
  if (Serial.available() > 0) {
    int recv = Serial.read();
    switch (recv) {
      case '#': //command start
        memset(buf, 0, sizeof(buf));
        i = 0;
        break;
      case '$': //command end
        handleCommand(buf);
      default:
        buf[i++] = (char) recv;
    }
  }

  for (int j = 0; j < 3; j++) {
    if (nop[j] > 0) {
      nop[j]--;
    } else {
      int e = target[j] - pos[j];
      if (e != 0) {
        e = e / abs(e);
      }
      if (abs(e) > 0) {
        pos[j] += e;
        moveServo(j, pos[j]);
      }
      nop[j] = 50 * (180 / (abs(target[j] - pos[j]) + 1));
    }
  }

}

void handleCommand(char* buf) {
  if (buf[0] == 'L') {
    handleLed(buf[2] - '0');    
  } else if (buf[0] == 'S') {
    handleServo(&buf[2]);
  } else {
    Serial.println("Unknown command");
  }  
}

void handleLed(char enabled) {
  digitalWrite(LED_PIN, enabled?HIGH:LOW);  
}

void handleServo(char* buf) {
  struct t_command cmd = parseBuffer(buf);
  target[cmd.index] = cmd.value;
}

struct t_command parseBuffer(char* buf) {
  String str(buf);
  int index = String(str[0]).toInt();
  int value = str.substring(str.indexOf(':') + 1).toInt();
  return {index, value};
}

void moveServo(int index, int value) {
  pwm.setPWM(index, 0, map(value, 0, 180, SERVOMIN, SERVOMAX));
}

