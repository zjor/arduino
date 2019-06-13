#define DIR_A 12
#define PWM_A 3

#define OUTPUT_A  4
#define OUTPUT_B  2

// pulses per revolution
#define PPR   240

#define Kp  12.0
#define Kd  2.5
#define Ki  0.1

volatile long encoderPosition = 0;
long lastEncoderPosition = 0;

// reference speed in RPS
float ref_w = -3.0;

// measured speed in RPS
float est_w = .0;
float last_w = .0;
float accel = .0;
float error_acc = .0;
float last_error = .0;

int dir = HIGH;
float pwm = .0;

void setup() {
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);

  pinMode(OUTPUT_A, INPUT_PULLUP);
  pinMode(OUTPUT_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), aHandler, RISING); 
  attachInterrupt(digitalPinToInterrupt(OUTPUT_B), bHandler, RISING);  

  Serial.begin(115200);
  
  analogWrite(PWM_A, int(pwm));
  digitalWrite(DIR_A, dir);
}

unsigned long lastEncoderUpdateTime = 0;

void measure(unsigned long dt) {
  unsigned long now = millis();
  if (now - lastEncoderUpdateTime >= dt) {
    est_w = 1000.0 * (encoderPosition - lastEncoderPosition) / PPR / (now - lastEncoderUpdateTime);
    accel = 1000.0 * (est_w - last_w) / (now - lastEncoderUpdateTime);
    last_w = est_w;
    
    lastEncoderUpdateTime = now;
    lastEncoderPosition = encoderPosition;    
  }  
}

float getControl(unsigned long dt) {
  float error = ref_w - est_w;
  error_acc += error * dt / 1000;

  float u = Kp * error + 1000.0 * Kd * (error - last_error) / dt + Ki * error_acc;
  last_error = error;
  return u;  
}

unsigned long lastControlUpdateTime = 0;

void applyControl(unsigned long dt) {
  if (millis() - lastControlUpdateTime >= dt) {
    float u = getControl(dt);
    pwm += u;
    analogWrite(PWM_A, abs(int(pwm)));

    if (pwm > 0) {
      digitalWrite(DIR_A, HIGH);
    } else {
      digitalWrite(DIR_A, LOW);
    }
    
    lastControlUpdateTime = millis();  
  }
  
}

unsigned long lastLogTime = 0;

void logState(unsigned long dt) {
  unsigned long now = millis();
  if (now - lastLogTime >= dt) {

    Serial.print(ref_w);
    Serial.print('\t');
    Serial.println(est_w);
//    Serial.print('\t');
//    Serial.println(accel);
        
    lastLogTime = now;  
  }
}

unsigned long lastRandomizedTime = 0;

void randomizeRef(unsigned long dt) {
  unsigned long now = millis();
  if (now - lastRandomizedTime >= dt) {
    ref_w = random(-5, 5);
    error_acc = .0;
    lastRandomizedTime = now;  
  }
}


void loop() {
  if (millis() > 15 * 1000) {
    analogWrite(PWM_A, 0);
    return;    
  }
  
  randomizeRef(5000);
  
  logState(50);
  measure(25);

//  return;
  applyControl(40);
  
}

void aHandler() {
  if (digitalRead(OUTPUT_B) == LOW) {
    encoderPosition++;
  } else {
    encoderPosition--;
  }
}

void bHandler() {
  if (digitalRead(OUTPUT_A) == LOW) {
    encoderPosition--;
  } else {
    encoderPosition++;
  }
}
