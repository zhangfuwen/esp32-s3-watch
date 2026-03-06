/**
 * WiFi Driver Header
 * 
 * @file wifi.h
 * @brief WiFi connectivity interface
 */

#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initialize WiFi subsystem
 * @return ESP_OK on success
 */
esp_err_t wifi_init(void);

/**
 * @brief Connect to WiFi network
 * @param ssid Network SSID
 * @param password Network password
 * @return ESP_OK on success
 */
esp_err_t wifi_connect(const char *ssid, const char *password);

/**
 * @brief Check WiFi connection status
 * @return true if connected
 */
bool wifi_is_connected(void);

#endif // WIFI_H
