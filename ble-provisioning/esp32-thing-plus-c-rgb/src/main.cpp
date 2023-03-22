#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <aWOT.h>

#include "StaticFiles.h"
#include "provisioning/BLEProvisioning.h"

BLEProvisioning bleProvisioning;

WiFiServer server(80);
Application app;

uint8_t r = 0, g = 0, b = 0;

void cors_options(Request &req, Response &res) {
  res.set("Access-Control-Allow-Origin", "*");
  res.set("Access-Control-Allow-Headers", "*");
  res.set("Access-Control-Allow-Methods", "GET, HEAD, POST, OPTIONS");
  
  res.sendStatus(204);
}

void getRGBStatusHandler(Request &req, Response &res) {
  DynamicJsonDocument json(1024);
  json["r"] = r;
  json["g"] = g;
  json["b"] = b;

  res.set("Access-Control-Allow-Origin", "*");
  res.set("Access-Control-Allow-Headers", "*");
  res.set("Access-Control-Allow-Methods", "GET, HEAD, POST, OPTIONS");
  res.set("Content-Type", "application/json");

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

  res.set("Access-Control-Allow-Origin", "*");
  res.set("Access-Control-Allow-Headers", "*");
  res.set("Access-Control-Allow-Methods", "GET, HEAD, POST, OPTIONS");
  res.set("Content-Type", "application/json");

  res.print(json.as<String>());

  neopixelWrite(RGB_BUILTIN, r, g, b);
}

void wifi_event_handler(arduino_event_t *event) {  
  switch (event->event_id) {
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    bleProvisioning.set_wifi_status(STATUS_CONNECTED);
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    bleProvisioning.set_wifi_status(STATUS_DISCONNECTED);
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    bleProvisioning.set_wifi_status(STATUS_GOT_IP);
    break;
  }
}

void connect_to_wifi(void *param) {
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(wifi_event_handler, ARDUINO_EVENT_MAX);
  WiFi.begin(bleProvisioning.wifi_ssid.c_str(),
             bleProvisioning.wifi_password.c_str());
  bleProvisioning.set_wifi_status(STATUS_CONNECTING);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.printf("\nGo to: http://%s\n", WiFi.localIP().toString().c_str());

  bleProvisioning.set_ip_address(WiFi.localIP().toString().c_str());

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

  app.use(staticFiles());
  app.options(&cors_options);  
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