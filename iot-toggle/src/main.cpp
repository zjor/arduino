#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "credentials.h"

const char *url = "https://mqtt2telegram.projects.royz.cc/api/v1.0/send";
const char *JSON_TEMPLATE = "{\"topic\":\"%s\",\"payload\":\"%s\"}";
const char *topic = "toggle/101";

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

void setup() {

  Serial.begin(115200);

  wl_status_t wifi_status = WiFi.status();

  Serial.println("Connecting to WiFi...");

  while (wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");    
    delay(5000);
  }

  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  send_message(topic, "Hello world");

}

void loop() {
  
}