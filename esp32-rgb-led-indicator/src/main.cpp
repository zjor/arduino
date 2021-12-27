#include <Arduino.h>

#define R_LED_PIN 32
#define G_LED_PIN 33
#define B_LED_PIN 25

#define R_CH 0
#define G_CH 1
#define B_CH 2

#define FSM_STATE_OFF 0
#define FSM_STATE_ON  1

#define FSM_STATE_A   0
#define FSM_STATE_B   1
#define FSM_STATE_C   2

#define FSM_DELAY_FAST    250
#define FSM_DELAY_MEDIUM  500
#define FSM_DELAY_SLOW    750

typedef struct {
  uint8_t next;
  unsigned long delay_millis;
  uint32_t color;
} fsm_transition_t;

fsm_transition_t fsm1[] = {
  { FSM_STATE_ON, FSM_DELAY_MEDIUM, 0xFF0000 },
  { FSM_STATE_OFF, FSM_DELAY_MEDIUM, 0x0000FF },
};

fsm_transition_t fsm2[] = {
  { FSM_STATE_B, FSM_DELAY_SLOW, 0xFF0000 },
  { FSM_STATE_C, FSM_DELAY_SLOW, 0xFFFF00 },
  { FSM_STATE_A, FSM_DELAY_SLOW, 0x00FF00 },
};

fsm_transition_t *fsm = fsm2;
uint8_t current_state = FSM_STATE_OFF;
unsigned long last_entered_ts = 0L;

void configure_led() {
  ledcAttachPin(R_LED_PIN, R_CH);
  ledcAttachPin(G_LED_PIN, G_CH);
  ledcAttachPin(B_LED_PIN, B_CH);

  ledcSetup(R_CH, 1000, 8);
  ledcSetup(G_CH, 1000, 8);
  ledcSetup(B_CH, 1000, 8);  
}

void led_render_state() {
  uint32_t color = fsm[current_state].color;
  int red = (color & 0xFF0000) >> 16;
  int green = (color & 0x00FF00) >> 8;
  int blue = (color & 0x0000FF);

  Serial.print("r: ");
  Serial.print(red);
  Serial.print("\t");

  Serial.print("g: ");
  Serial.print(green);
  Serial.print("\t");

  Serial.print("b: ");
  Serial.println(blue);

  ledcWrite(R_CH, 255 - red);
  ledcWrite(G_CH, 255 - green);
  ledcWrite(B_CH, 255 - blue);
}

unsigned long last_fsm_change = 0L;

void setup() {
  Serial.begin(115200);
  configure_led();

  last_entered_ts = millis();
  last_fsm_change = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - last_entered_ts >= fsm[current_state].delay_millis) {
    current_state = fsm[current_state].next;
    last_entered_ts = now;
  }

  led_render_state();

  if (now - last_fsm_change >= 5000) {
    if (fsm == fsm1) {
      fsm = fsm2;
    } else {
      fsm = fsm1;
    }
    last_fsm_change = now;
  }
}