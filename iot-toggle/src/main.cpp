#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <EEPROM.h>

#include "credentials.h"
#include "bot_client.h"
#include "hue_led.h"
#include "credentials_storage.h"
#include "BLEProvisioning.h"

#define R_PIN 14
#define G_PIN 15
#define B_PIN 27

// busy/idle toggle
#define SWITCH_A_PIN  32

// SmartConfig toggle
#define SWITCH_B_PIN  33

// FSM definition

typedef enum {
  STARTED             = -1,
  CONNECTING          = 0,
  ONLINE              = 1,
  TOGGLE_IDLE         = 2,
  TOGGLE_BUSY         = 3,
  BLE_PROVISIONING    = 4
} device_status_t;

device_status_t device_status = STARTED;

void set_status(device_status_t new_status);

/* status handlers */
void handle_connecting_state();
void handle_online_state();
void handle_idle_state();
void handle_busy_state();
void handle_ble_provisioning_state();

/* transition handlers */
void on_connecting();
void on_online();
void on_idle();
void on_busy();
void on_start_ble_provisioning();

void (*handlers[])() = {
  handle_connecting_state,
  handle_online_state,
  handle_idle_state,
  handle_busy_state,
  handle_ble_provisioning_state
};

// end of FSM definition

BLEProvisioning bleProvisioning;

HueLed led(R_PIN, G_PIN, B_PIN);

const char *MQTT_TOPIC = "toggle/101";

BotClient bot(BOT_LOGIN, BOT_PASSWORD);

unsigned long now_millis = 0;
unsigned long toggle_elapsed_millis = 0;

void wifi_event_handler(arduino_event_t *event) {  
  switch (event->event_id) {
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    bleProvisioning.set_wifi_status(STATUS_CONNECTED);
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    bleProvisioning.set_bad_credentials();
    bleProvisioning.set_wifi_status(STATUS_DISCONNECTED);
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    bleProvisioning.set_wifi_status(STATUS_GOT_IP);
    break;
  }
}

void onCredentialsReady() {
  WiFi.onEvent(wifi_event_handler, ARDUINO_EVENT_MAX);
  const char *ssid = bleProvisioning.wifi_ssid.c_str();
  const char *password = bleProvisioning.wifi_password.c_str();
  save_credentials(ssid, password);
  bleProvisioning.set_wifi_status(STATUS_CONNECTING);
  set_status(CONNECTING);
}

void update_led(int toggle_state) {
  if (toggle_state == LOW /* idle */) {
    led.set_rgb(0, 255, 0);
  } else {
    led.set_rgb(255, 0, 0);  
  }
}

void setup() {
  WiFi.mode(WIFI_STA);
  EEPROM.begin(EEPROM_SIZE);
  led.init();

  pinMode(SWITCH_A_PIN, INPUT_PULLUP);
  pinMode(SWITCH_B_PIN, INPUT_PULLUP);
  
  Serial.begin(115200);

  if (has_credentials()) {
    set_status(CONNECTING);
    // TODO: set creds to BLE
  } else {
    set_status(BLE_PROVISIONING);
  }

  bleProvisioning.set_on_credentials_ready_callback(&onCredentialsReady);
  bleProvisioning.init("ESP32 :: IoT Toggle");

  now_millis = millis();
}

void loop() {
  now_millis = millis();
  if (device_status != STARTED) {
    (*handlers[device_status])();
  }

  delay(10);
}

void set_status(device_status_t new_status) {
  switch (new_status) {
    case STARTED:
      break;
    case CONNECTING:
      on_connecting(); break;
    case ONLINE:
      on_online(); break;      
    case TOGGLE_IDLE:
      on_idle(); break;
    case TOGGLE_BUSY:
      on_busy(); break;
    case BLE_PROVISIONING:
      on_start_ble_provisioning(); break;
  }
  Serial.printf("Status transition: %d -> %d\n", device_status, new_status);
  device_status = new_status;
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

void on_connecting() {
  char ssid[32], password[32];
  load_credentials(ssid, password);

  Serial.println("Connecting to WiFi with credentials:");
  Serial.printf("  SSID: %s\n", ssid);
  Serial.printf("  Password: %s\n", password);

  WiFi.begin(ssid, password);
}

void on_start_ble_provisioning() {
  erase_credentials();
}

void on_online() {
  IPAddress ipAddress = WiFi.localIP();
  bleProvisioning.set_ip_address(ipAddress.toString().c_str());

  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(ipAddress);

  bot.send_message(MQTT_TOPIC, "online");
}

void on_idle() {  
  unsigned long elapsed_millis = now_millis - toggle_elapsed_millis;
  toggle_elapsed_millis = now_millis;
  update_led(LOW);

  char buf[64];
  char time_buf[16];
  format_time(elapsed_millis, time_buf);

  sprintf(buf, "[idle] busy time: %s (%ldms)", time_buf, elapsed_millis);
  bot.send_message(MQTT_TOPIC, buf);
  Serial.println(buf);
}

void on_busy() {
  unsigned long elapsed_millis = now_millis - toggle_elapsed_millis;
  toggle_elapsed_millis = now_millis;

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
  hue = (hue + 1) % 255;
}

void blink_blue() {
  static unsigned int i = 0;
  i++;
  led.set_rgb(0, 0, ((i / 30) % 2 == 0) ? 255 : 0);
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
    set_status(ONLINE);
  } else {
    print_dot_progress();
    hue_rotate();
  }

  if (digitalRead(SWITCH_B_PIN) == LOW) {
    set_status(BLE_PROVISIONING);
  }

}

void handle_online_state() {
  int toggle_state = digitalRead(SWITCH_A_PIN);
  toggle_elapsed_millis = now_millis;

  set_status(toggle_state == LOW ? TOGGLE_IDLE : TOGGLE_BUSY);
}

void handle_idle_state() {
  wl_status_t wifi_status = WiFi.status();
  if (wifi_status != WL_CONNECTED) {
    set_status(CONNECTING);
    return;
  }

  if (digitalRead(SWITCH_B_PIN) == LOW) {
    set_status(BLE_PROVISIONING);
  }

  int toggle_state = digitalRead(SWITCH_A_PIN);
  if (toggle_state == LOW) {
    // do nothing we are idle
  } else {
    set_status(TOGGLE_BUSY);
  }
}

void handle_busy_state() {
  wl_status_t wifi_status = WiFi.status();
  if (wifi_status != WL_CONNECTED) {
    set_status(CONNECTING);
    return;
  }

  if (digitalRead(SWITCH_B_PIN) == LOW) {
    set_status(BLE_PROVISIONING);
  }

  int toggle_state = digitalRead(SWITCH_A_PIN);
  if (toggle_state == LOW) {
    set_status(TOGGLE_IDLE);
  } else {
    // do nothing we are busy
  }
}

void handle_ble_provisioning_state() {
    print_dot_progress();
    blink_blue();
}