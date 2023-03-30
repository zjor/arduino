#ifndef _CREDENTIALS_STORAGE_H_
#define _CREDENTIALS_STORAGE_H_

#define EEPROM_SIZE 64

bool has_credentials();
void save_credentials(const char *ssid, const char *pass);
void load_credentials(char *ssid, char *pass);
void erase_credentials();

#endif