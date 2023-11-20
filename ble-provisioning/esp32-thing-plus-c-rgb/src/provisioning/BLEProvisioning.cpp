#include "BLEProvisioning.h"

BLEProvisioning::BLEProvisioning(){
  bool _ssid_set = false;
  bool _password_set = false;
  uint8_t _credentials_status = 0;
  _ip_char = nullptr;
  _status_char = nullptr;
  _char_callback = nullptr;
}

BLEProvisioning::~BLEProvisioning(){
  delete _ip_char;
  delete _status_char;
  delete _char_callback;
}

void BLEProvisioning::create_wifi_ssid_characteristic(BLEService *service){
  BLECharacteristic *c = service->createCharacteristic(
      BP_WIFI_SSID_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  c->setCallbacks(this);
  c->setValue("SSID");
  delete c;
}

void BLEProvisioning::create_wifi_password_characteristic(BLEService *service){
  BLECharacteristic *c = service->createCharacteristic(
      BP_WIFI_PASS_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  c->setCallbacks(this);
  c->setValue("Password");
  delete c;
}

BLECharacteristic * BLEProvisioning::create_wifi_status_characteristic(BLEService *service) {
  BLECharacteristic *c = service->createCharacteristic(
      BP_WIFI_STATUS_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  c->setValue(STATUS_READY);
  return c;
}

BLECharacteristic * BLEProvisioning::create_ip_characteristic(BLEService *service) {
  BLECharacteristic *c = service->createCharacteristic(
      BP_IP_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  c->setValue("");
  return c;
}

void BLEProvisioning::onWrite(BLECharacteristic *c) {
  if (!c->getUUID().toString().compare(BP_WIFI_SSID_CHAR_UUID)) {
    wifi_ssid = c->getValue();
    _credentials_status |= 1 << WIFI_SSID_SET_BIT;
    Serial.print("SSID set: ");
    Serial.println(this->wifi_ssid.c_str());
  } else if (!c->getUUID().toString().compare(BP_WIFI_PASS_CHAR_UUID)) {
    wifi_password = c->getValue();
    _credentials_status |= 1 << WIFI_PASS_SET_BIT;
    Serial.print("Password set: ");
    Serial.println(wifi_password.c_str());
  }
  if (_credentials_status & (1 << WIFI_SSID_SET_BIT) && 
      _credentials_status & (1 << WIFI_PASS_SET_BIT)) {
    _credentials_status = 0;
    _callback();
  }
}

void BLEProvisioning::init(const char *device_name) {
  BLEDevice::init(device_name);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BP_SERVICE_UUID);
  create_wifi_ssid_characteristic(pService);
  create_wifi_password_characteristic(pService);
  this->_status_char = create_wifi_status_characteristic(pService);
  this->_ip_char = create_ip_characteristic(pService);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BP_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  delete pServer;
  delete pService;
  delete pAdvertising;
}

void BLEProvisioning::set_on_credentials_ready_callback(t_void_func callback) {
  this->_callback = callback;
};

void BLEProvisioning::set_ip_address(const char *ip_address) {
  _ip_char->setValue(ip_address);
  _ip_char->notify();
}

void BLEProvisioning::set_wifi_status(const char *status) {
  _status_char->setValue(status);
  _status_char->notify();
}

void BLEProvisioning::set_bad_credentials() {
  _credentials_status |= (1 << WIFI_BAD_CREDENTIALS_BIT);
}

bool BLEProvisioning::is_bad_credentials() {
  return _credentials_status & (1 << WIFI_BAD_CREDENTIALS_BIT);
}

std::string BLEProvisioning::get_wifi_ssid(){
  return wifi_ssid;
}

std::string BLEProvisioning::get_wifi_password(){
  return wifi_password;
}

