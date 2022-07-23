#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "credentials.h"
#include "bot_client.h"
#include "hue_led.h"

#define R_PIN 14
#define G_PIN 15
#define B_PIN 27

#define SWITCH_A_PIN  12
#define SWITCH_B_PIN  13

// FSM definition

typedef enum {
  CONNECTING          = 0,
  ONLINE              = 1,
  TOGGLE_IDLE         = 2,
  TOGGLE_BUSY         = 3
} device_status_t;

device_status_t device_status = CONNECTING;

void handle_connecting_state();
void handle_online_state();
void handle_idle_state();
void handle_busy_state();

void (*handlers[])() = {
  handle_connecting_state,
  handle_online_state,
  handle_idle_state,
  handle_busy_state
};

// end of FSM definition

HueLed led(R_PIN, G_PIN, B_PIN);

const char *MQTT_TOPIC = "toggle/101";

BotClient bot(BOT_LOGIN, BOT_PASSWORD);

unsigned long toggle_elapsed_millis;

void update_led(int toggle_state) {
  if (toggle_state == LOW /* idle */) {
    led.set_rgb(0, 255, 0);
  } else {
    led.set_rgb(255, 0, 0);  
  }
}

void setup() {
  led.init();

  pinMode(SWITCH_A_PIN, INPUT_PULLUP);
  pinMode(SWITCH_B_PIN, INPUT_PULLUP);
  
  Serial.begin(115200);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  (*handlers[device_status])();

  delay(10);
}

void on_idle(unsigned long elapsed_millis) {
  update_led(LOW);

  char buf[64];
  sprintf(buf, "[idle] busy time: %ldms", elapsed_millis);
  bot.send_message(MQTT_TOPIC, buf);
  Serial.println(buf);
}

void on_busy(unsigned long elapsed_millis) {
  update_led(HIGH);

  char buf[64];
  sprintf(buf, "[busy] idle time: %ldms", elapsed_millis);
  bot.send_message(MQTT_TOPIC, buf);
  Serial.println(buf);
}

void handle_connecting_state() {
  static int _i = 0;
  static int hue = 0; 
  wl_status_t wifi_status = WiFi.status();  
  if (wifi_status == WL_CONNECTED) {
    device_status = ONLINE;
  } else {
    if (_i % 5 == 0) {
      Serial.print(".");
    }
    _i++;
    led.set_hsv(hue, 255, 255);
    hue = (++hue) % 255;    
  }
}

void handle_online_state() {
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.send_message(MQTT_TOPIC, "online");

  int toggle_state = digitalRead(SWITCH_A_PIN);
  if (toggle_state == LOW) {
    on_idle(0L);
    device_status = TOGGLE_IDLE;    
  } else {
    on_busy(0L);
    device_status = TOGGLE_BUSY;    
  }

  toggle_elapsed_millis = millis();
}

void handle_idle_state() {
  int toggle_state = digitalRead(SWITCH_A_PIN);
  if (toggle_state == LOW) {
    // do nothing we are idle
  } else {
    unsigned long now = millis();
    on_busy(now - toggle_elapsed_millis);
    device_status = TOGGLE_BUSY;
    toggle_elapsed_millis = now;
  }
}

void handle_busy_state() {
  int toggle_state = digitalRead(SWITCH_A_PIN);
  if (toggle_state == LOW) {
    unsigned long now = millis();
    on_idle(now - toggle_elapsed_millis);
    device_status = TOGGLE_IDLE;
    toggle_elapsed_millis = now;
  } else {
    // do nothing we are busy
  }
}
