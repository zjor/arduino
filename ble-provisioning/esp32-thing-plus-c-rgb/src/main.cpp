#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <aWOT.h>

#include "provisioning/BLEProvisioning.h"

BLEProvisioning bleProvisioning;

WiFiServer server(80);
Application app;

uint8_t r = 0, g = 0, b = 0;

void index(Request &req, Response &res) { res.print("Hello Wolrd!"); }

void getRGBStatusHandler(Request &req, Response &res) {
  DynamicJsonDocument json(1024);
  json["r"] = r;
  json["g"] = g;
  json["b"] = b;
  res.print(json.as<String>());
}

void postRGBStatusHandler(Request &req, Response &res) {
  DynamicJsonDocument json(1024);
  deserializeJson(json, req);
  r = json["r"];
  g = json["g"];
  b = json["b"];
  json["r"] = r;
  json["g"] = g;
  json["b"] = b; // convert values to int
  res.print(json.as<String>());

  neopixelWrite(RGB_BUILTIN, r, g, b);
}

void wifi_event_handler(arduino_event_t *event) {  
  switch (event->event_id) {
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    bleProvisioning.set_wifi_status("connected");
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    bleProvisioning.set_wifi_status("disconnected");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    bleProvisioning.set_wifi_status("got IP");
    break;
  }
}

void connect_to_wifi(void *param) {
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(wifi_event_handler, ARDUINO_EVENT_MAX);
  WiFi.begin(bleProvisioning.wifi_ssid.c_str(),
             bleProvisioning.wifi_password.c_str());
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.printf("\nGo to: http://%s\n", WiFi.localIP().toString().c_str());

  std::string ip_value = "IP: ";
  ip_value += WiFi.localIP().toString().c_str();
  bleProvisioning.set_ip_address(ip_value.c_str());

  server.begin();

  vTaskDelete(NULL);
}

void onCredentialsReady() {
  xTaskCreate(connect_to_wifi, "ConnectTask", 10000, NULL, 1, NULL);
}


void setup() {
  Serial.begin(115200);
  digitalWrite(RGB_BUILTIN, HIGH); // enable built-in WS2812C RGB led

  bleProvisioning.set_on_credentials_ready_callback(&onCredentialsReady);
  bleProvisioning.init("ESP32 Thing Plus-C");

  app.get("/", &index);
  app.get("/api/rgb", &getRGBStatusHandler);
  app.post("/api/rgb", &postRGBStatusHandler);

  neopixelWrite(RGB_BUILTIN, r, g, b);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    return;
  }

  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
  }
}