#include <Arduino.h>
#include <EEPROM.h>

#include "credentials_storage.h"

bool has_credentials() {
  return EEPROM.read(0) == 0x42 /* credentials marker */;
}

void save_credentials(const char *ssid, const char *pass) {
  char buf[EEPROM_SIZE];
  sprintf(buf, "%c%s:%s", 0x42, ssid, pass);
  EEPROM.writeString(0, buf);
  EEPROM.commit();
}

void load_credentials(char *ssid, char *pass) {
  if (!has_credentials()) {
    return;
  }
  char buf[EEPROM_SIZE];
  EEPROM.readString(1, buf, EEPROM_SIZE - 1);
  int i = 0;
  while ( i < EEPROM_SIZE && *(buf + i) != ':') {
    *ssid = *(buf + i);
    ssid++;
    i++;
  }
  *ssid = '\0';
  if (i == EEPROM_SIZE) {
    return;
  }
  i++;

  while ( i < EEPROM_SIZE && *(buf + i) != '\0') {
    *pass = *(buf + i);
    pass++;
    i++;
  }
  *pass = '\0';
}

void erase_credentials() {
  EEPROM.write(0, 0);
  EEPROM.commit();
}