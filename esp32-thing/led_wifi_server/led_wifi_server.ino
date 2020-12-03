#include <WiFi.h>
#include <aWOT.h>

#include "static_index.h"

#define LED_PIN 13

WiFiServer server(80);
Application app;

int ledStatus = LOW;

void index(Request &req, Response &res) {
  res.set("Content-Type", "text/html");
  res.writeP(static_index, 1168);  
}

void toggleLed(Request &req, Response &res) {
  ledStatus = !ledStatus;
  digitalWrite(LED_PIN, ledStatus);
  
  res.set("Content-Type", "application/json");
  String out = "{\"led_on\": ";
  res.print(out + (ledStatus == HIGH ? "true" : "false") + "}");
}

const char* ssid = "<>";
const char* password = "<>";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  app.get("/", &index);
  app.post("/toggle", &toggleLed);
  server.begin();
}
  
void loop() {  
  WiFiClient client = server.available();
  
  if (client.connected()) {
    app.process(&client);
  }
}
