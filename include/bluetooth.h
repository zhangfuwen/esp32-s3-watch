/**
 * Bluetooth Driver Header
 * 
 * @file bluetooth.h
 * @brief Bluetooth LE interface
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_err.h"

/**
 * @brief Initialize Bluetooth subsystem
 * @return ESP_OK on success
 */
esp_err_t bluetooth_init(void);

/**
 * @brief Start BLE advertising
 * @return ESP_OK on success
 */
esp_err_t bluetooth_start_advertising(void);

/**
 * @brief Stop BLE advertising
 * @return ESP_OK on success
 */
esp_err_t bluetooth_stop_advertising(void);

#endif // BLUETOOTH_H
