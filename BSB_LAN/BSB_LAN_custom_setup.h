// Add custom code for setup function here which will be included at the end of the function

#include <Arduino.h>

// #include "custom_functions/Update_firmware_from_SD/BSB_LAN_custom_setup.h"

// Power up DS18B20 3.3v PIN
pinMode(2, OUTPUT);
digitalWrite(2, HIGH);

// BLE to max power
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9); 
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);