#include <Arduino.h>
#include <HTTPClient.h>

#include "bot_client.h"

const char *url = "https://mqtt2telegram.projects.royz.cc/api/v1.0/send";
const char *JSON_TEMPLATE = "{\"topic\":\"%s\",\"payload\":\"%s\"}";

BotClient::BotClient(const char *login, const char *password) {
  this->login = login;
  this->password = password;
}

void BotClient::send_message(const char *topic, const char *payload) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(this->login, this->password);

  char requestBody[256];
    
  snprintf_P(requestBody, sizeof(requestBody), PSTR(JSON_TEMPLATE), topic, payload);
  int responseCode = http.POST(requestBody);
  Serial.println(responseCode);
}