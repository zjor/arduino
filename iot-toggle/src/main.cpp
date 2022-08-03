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
  TOGGLE_BUSY         = 3,
  SMART_CONFIG        = 4
} device_status_t;

device_status_t device_status = SMART_CONFIG;

void handle_connecting_state();
void handle_online_state();
void handle_idle_state();
void handle_busy_state();
void handle_smart_config_state();

void (*handlers[])() = {
  handle_connecting_state,
  handle_online_state,
  handle_idle_state,
  handle_busy_state,
  handle_smart_config_state
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

  // Serial.println("Connecting to WiFi...");
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Starting SmartConfig");
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
}

void loop() {
  (*handlers[device_status])();

  delay(10);
}

void format_time(unsigned long t, char *buf) {
	long ms = t % 1000;
	t = t / 1000;
	long seconds = t % 60;
	t = t / 60;
	long mins = t % 60;
	long hours = t / 60;
  sprintf(buf, "%02ld:%02ld:%02ld.%03ld", hours, mins, seconds, ms);
}

void on_idle(unsigned long elapsed_millis) {
  update_led(LOW);

  char buf[64];
  char time_buf[16];
  format_time(elapsed_millis, time_buf);

  sprintf(buf, "[idle] busy time: %s (%ldms)", time_buf, elapsed_millis);
  bot.send_message(MQTT_TOPIC, buf);
  Serial.println(buf);
}

void on_busy(unsigned long elapsed_millis) {
  update_led(HIGH);

  char buf[64];
  char time_buf[16];
  format_time(elapsed_millis, time_buf);

  sprintf(buf, "[busy] idle time: %s (%ldms)", time_buf, elapsed_millis);
  bot.send_message(MQTT_TOPIC, buf);
  Serial.println(buf);
}

void hue_rotate() {
  static unsigned int hue = 0;
  led.set_hsv(hue, 255, 255);
  hue = (++hue) % 255;
}

void print_dot_progress() {
  static int i = 0;
  if (i % 10 == 0) {
    Serial.print(".");
  }
  if (i % 1000 == 0) {
    Serial.println();
  }
  i++;
}

void handle_connecting_state() {
  wl_status_t wifi_status = WiFi.status();
  if (wifi_status == WL_CONNECTED) {
    device_status = ONLINE;
  } else {
    print_dot_progress();
    hue_rotate();
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
  wl_status_t wifi_status = WiFi.status();
  if (wifi_status != WL_CONNECTED) {
    device_status = CONNECTING;
    return;
  }

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
  wl_status_t wifi_status = WiFi.status();
  if (wifi_status != WL_CONNECTED) {
    device_status = CONNECTING;
    return;
  }

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

void handle_smart_config_state() {
  if (!WiFi.smartConfigDone()) {
    print_dot_progress();
    hue_rotate();
  } else {
    Serial.println("SmartConfig received");
    device_status = CONNECTING;
  }

}