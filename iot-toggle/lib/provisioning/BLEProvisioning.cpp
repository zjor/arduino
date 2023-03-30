#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "BLEProvisioning.h"

void create_wifi_ssid_characteristic(BLEService *service,
                                     BLECharacteristicCallbacks *pCallbacks) {
  BLECharacteristic *c = service->createCharacteristic(
      BP_WIFI_SSID_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  c->setCallbacks(pCallbacks);
  c->setValue("SSID");
}

void create_wifi_password_characteristic(
    BLEService *service, BLECharacteristicCallbacks *pCallbacks) {
  BLECharacteristic *c = service->createCharacteristic(
      BP_WIFI_PASS_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  c->setCallbacks(pCallbacks);
  c->setValue("Password");
}

BLECharacteristic *create_wifi_status_characteristic(BLEService *service) {
  BLECharacteristic *c = service->createCharacteristic(
      BP_WIFI_STATUS_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  c->setValue(STATUS_READY);
  // c->notify();
  return c;
}

BLECharacteristic *create_ip_characteristic(BLEService *service) {
  BLECharacteristic *c = service->createCharacteristic(
      BP_IP_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  c->setValue("");
  // c->notify();
  return c;
}

void BLEProvisioning::set_on_credentials_ready_callback(t_void_func callback) {
  this->_callback = callback;
};

void BLEProvisioning::init(const char *device_name) {
  BLEDevice::init(device_name);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BP_SERVICE_UUID);
  create_wifi_ssid_characteristic(pService, this);
  create_wifi_password_characteristic(pService, this);
  this->_status_char = create_wifi_status_characteristic(pService);
  this->_ip_char = create_ip_characteristic(pService);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BP_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(
      0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void BLEProvisioning::onWrite(BLECharacteristic *c) {

  if (c->getUUID().toString().compare(BP_WIFI_SSID_CHAR_UUID) == 0) {
    this->wifi_ssid = c->getValue();
    this->_credentials_status |= 1 << WIFI_SSID_SET_BIT;
    
    Serial.print("SSID set: ");
    Serial.println(this->wifi_ssid.c_str());
  } else if (c->getUUID().toString().compare(BP_WIFI_PASS_CHAR_UUID) == 0) {
    this->wifi_password = c->getValue();
    this->_credentials_status |= 1 << WIFI_PASS_SET_BIT;

    Serial.print("Password set: ");
    Serial.println(this->wifi_password.c_str());
  }

  if (this->_credentials_status & (1 << WIFI_SSID_SET_BIT) && 
      this->_credentials_status & (1 << WIFI_PASS_SET_BIT)) {
    this->_credentials_status = 0;
    this->_callback();
  }
}

void BLEProvisioning::set_ip_address(const char *ip_address) {
  this->_ip_char->setValue(ip_address);
  this->_ip_char->notify();
}

void BLEProvisioning::set_wifi_status(const char *status) {
  this->_status_char->setValue(status);
  this->_status_char->notify();
}

void BLEProvisioning::set_bad_credentials() {
  this->_credentials_status |= (1 << WIFI_BAD_CREDENTIALS_BIT);
}

bool BLEProvisioning::is_bad_credentials() {
  return this->_credentials_status & (1 << WIFI_BAD_CREDENTIALS_BIT);
}