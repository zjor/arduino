#include <SoftwareSerial.h>

#define MH_Z19_RX 7
#define MH_Z19_TX 6

SoftwareSerial sensorSerial(MH_Z19_RX, MH_Z19_TX);

void setup() {
  Serial.begin(9600);
  sensorSerial.begin(9600);

  Serial.println("Welcome to Hello MH-Z19 CO2 reading application.");
}

void loop() {
  Serial.println(readCO2());
  delay(1000);
}

int readCO2() {

  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

  byte response[9]; 

  sensorSerial.write(cmd, 9); 
  sensorSerial.readBytes(response, 9);

//  for (int i = 0; i < 9; i++) {
//    Serial.print(response[i], HEX);
//  }
//  
  
  if (response[0] != 0xFF) {
    Serial.print("Wrong starting byte from co2 sensor, got: ");    
    Serial.println(response[0], HEX);
    return -1;
  }

  if (response[1] != 0x86) {
    Serial.println("Wrong command from co2 sensor!");
    return -1;
  }

  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256 * responseHigh) + responseLow;
  return ppm;
}
