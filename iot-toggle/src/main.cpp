#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "credentials.h"

#define R_PIN 14
#define G_PIN 15
#define B_PIN 27

#define SWITCH_A_PIN  12
#define SWITCH_B_PIN  13

const char *url = "https://mqtt2telegram.projects.royz.cc/api/v1.0/send";
const char *JSON_TEMPLATE = "{\"topic\":\"%s\",\"payload\":\"%s\"}";
const char *topic = "toggle/101";

int toggle_a_state, toggle_b_state;

// TODO: move to bot_client library
void send_message(const char *topic, const char *payload) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(BOT_LOGIN, BOT_PASSWORD);

  char requestBody[256];
    
  snprintf_P(requestBody, sizeof(requestBody), PSTR(JSON_TEMPLATE), topic, payload);
  int responseCode = http.POST(requestBody);
  Serial.println(responseCode);
}

void update_led() {
  if (toggle_a_state == LOW) {
    digitalWrite(R_PIN, HIGH);
    digitalWrite(G_PIN, LOW);
  } else {
    digitalWrite(R_PIN, LOW);
    digitalWrite(G_PIN, HIGH);
  }
}

void setup() {

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  
  digitalWrite(R_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(B_PIN, HIGH);

  pinMode(SWITCH_A_PIN, INPUT_PULLUP);
  pinMode(SWITCH_B_PIN, INPUT_PULLUP);
  
  toggle_a_state = digitalRead(SWITCH_A_PIN);
  toggle_b_state = digitalRead(SWITCH_B_PIN);  

  Serial.begin(115200);

  wl_status_t wifi_status = WiFi.status();

  Serial.println("Connecting to WiFi...");

  static int b_state = LOW;
  while (wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    
    digitalWrite(B_PIN, b_state);
    b_state = !b_state;

    delay(5000);
  }

  digitalWrite(B_PIN, HIGH);

  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  send_message(topic, "online");
  update_led();
}

void loop() {
  int new_a_state = digitalRead(SWITCH_A_PIN);
  if (new_a_state != toggle_a_state) {
    toggle_a_state = new_a_state;
    update_led();

    send_message(topic, toggle_a_state == LOW ? "idle" : "busy");
  }

  delay(50);
  
}