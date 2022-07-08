#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "credentials.h"


void setup() {

  Serial.begin(115200);

  wl_status_t wifi_status = WiFi.status();

  Serial.println("Connecting to WiFi...");

  while (wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");    
    delay(1500);
  }

  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  HTTPClient http_client;
  // TODO: https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/examples/BasicHttpClient/BasicHttpClient.ino
  // http_client.begin("");
  // http_client.addHeader()
  // http_client.POST()

}

void loop() {
  
}