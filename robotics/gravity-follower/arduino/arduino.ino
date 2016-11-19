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

}

void handleCommand(char* buf) {
  Serial.print("Handling: ");
  Serial.println(buf);
  if (buf[0] == 'L') {
    handleLed(buf[2]);    
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
  moveServo(parseBuffer(buf));
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

