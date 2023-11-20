#ifndef BLE_PROVISIONING_H
#define BLE_PROVISIONING_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define BP_SERVICE_UUID          "3b8a3538-bd08-4124-8184-6ed1b17d3405"
#define BP_WIFI_SSID_CHAR_UUID   "0374b073-9979-40b5-81ab-c2d9840c7ed7"
#define BP_WIFI_PASS_CHAR_UUID   "fee597c1-6051-4247-8b1f-f5d1a7e6dce6"
#define BP_WIFI_STATUS_CHAR_UUID "961b1ff7-4259-4058-85cc-3435c85cd704"
#define BP_IP_CHAR_UUID          "489ab6fc-334b-4a0f-b11f-7dff297aebb4"

#define STATUS_READY          "READY"
#define STATUS_CONNECTING     "CONNECTING"
#define STATUS_CONNECTED      "CONNECTED"
#define STATUS_DISCONNECTED   "DISCONNECTED"
#define STATUS_GOT_IP         "GOT_IP"

#define WIFI_SSID_SET_BIT             0
#define WIFI_PASS_SET_BIT             1
#define WIFI_BAD_CREDENTIALS_BIT      2

typedef void (* t_void_func)();

class BLEProvisioning: public BLECharacteristicCallbacks {
  private:
    bool _ssid_set;
    bool _password_set;
    uint8_t _credentials_status;
    std::string wifi_ssid;
    std::string wifi_password;
    BLECharacteristic* _ip_char;
    BLECharacteristic* _status_char;
    BLECharacteristicCallbacks* _char_callback;
    t_void_func _callback;
    /**
     * @brief Method create BLE Characteristic, associated with wifi ssid.
     * @param[in] service - Service.
    */
    void create_wifi_ssid_characteristic(BLEService *service);
    /**
     * @brief Method create BLE Characteristic, associated with wifi password.
     * @param[in] service - Service.
    */
    void create_wifi_password_characteristic(BLEService *service);
    /**
     * @brief Method create BLE Characteristic, associated with wifi status.
     * @param[in] service - Service.
     * @return Characteristic of the wifi status.
    */
    BLECharacteristic *create_wifi_status_characteristic(BLEService *service);
    /**
     * @brief Method create BLE Characteristic, associated with ip characteristic.
     * @param[in] service - Service.
     * @return Characteristic of the wifi status.
    */
    BLECharacteristic *create_ip_characteristic(BLEService *service);
    /**
    * @brief Callback function to support a write request.
    * @param[in] c - The characteristic that is the source of the event.
    */
    void onWrite(BLECharacteristic* c) override;
  public:
    /**
     * @brief Constructor.
    */
    BLEProvisioning();
    /**
     * @brief Destructor.
    */
    ~BLEProvisioning();
    /**
     * @brief Get-method for wifi ssid.
     * @return Wifi SSID.
    */
    std::string get_wifi_ssid();
    /**
     * @brief Get-method for wifi password.
     * @return Wifi password.
    */
    std::string get_wifi_password();
    /**
     * @brief Method to initialize characteristics for new BLE device. 
     * @param[in] device_name - Name of the device.
    */
    void init(const char *device_name);
    /**
     * @brief Method to save new
     * @param[in] callback - Callback function.
    */
    void set_on_credentials_ready_callback(t_void_func callback);
    /**
     * @brief Method to set new ip address of the device.
     * @param[in] ip_address - Ip address.
    */
    void set_ip_address(const char*ip_address);
    /**
     * @brief Method to set new wifi status of the device.
     * @param[in] status - Wifi status.
    */
    void set_wifi_status(const char* status);
    /**
     * @brief Method to change the credentials status, in case connection has failed.
    */
    void set_bad_credentials();
    /**
     * @brief Method to check wifi credentials.
     * @return True - in case the wrong credentials has been set, in other cases - false.
    */
    bool is_bad_credentials();
};

#endif