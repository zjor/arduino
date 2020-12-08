#include <WiFi.h>
#include <aWOT.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "static_index.h"

#define SDA_PIN 23
#define SCL_PIN 22

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiServer server(80);
Application app;

const char* ssid = "<>";
const char* password = "<>";

void index(Request &req, Response &res) {
  res.set("Content-Type", "text/html");
  res.writeP(static_index, 1499);
}

void say(Request &req, Response &res) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(req.readString().c_str());
  display.display();
  res.print("Received");
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(F("Connecting..."));
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Connected"));
  display.println();
  display.print(F("Visit\nhttp://"));
  display.print(WiFi.localIP());
  display.display();

  app.get("/", &index);
  app.post("/say", &say);
  server.begin();


}
  
void loop() {
  WiFiClient client = server.available();
  
  if (client.connected()) {
    app.process(&client);
  }    
}
