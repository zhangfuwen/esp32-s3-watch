/**
 * ESP32-S3 Watch - Main Entry Point
 * 
 * @file main.c
 * @brief Main application entry point for ESP32-S3 smart watch
 * @version 0.1
 * @date 2026-03-06
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "display.h"
#include "watch_face.h"
#include "input.h"
#include "wifi.h"
#include "bluetooth.h"

static const char *TAG = "WATCH";

/**
 * @brief Main application entry point
 */
void app_main(void)
{
    ESP_LOGI(TAG, "=== ESP32-S3 Watch Starting ===");
    ESP_LOGI(TAG, "Version: 0.1.0");
    ESP_LOGI(TAG, "Build Date: %s %s", __DATE__, __TIME__);

    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized");

    // Initialize display
    ESP_LOGI(TAG, "Initializing display...");
    display_init();
    
    // Show boot screen
    display_clear();
    display_draw_text(10, 10, "ESP32-S3 Watch");
    display_draw_text(10, 30, "Version 0.1.0");
    display_update();
    
    // Initialize input system (buttons/touch)
    ESP_LOGI(TAG, "Initializing input...");
    input_init();
    
    // Initialize WiFi
    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init();
    
    // Initialize Bluetooth
    ESP_LOGI(TAG, "Initializing Bluetooth...");
    bluetooth_init();
    
    // Start watch face task
    ESP_LOGI(TAG, "Starting watch face...");
    watch_face_start();
    
    ESP_LOGI(TAG, "=== System Ready ===");
    
    // Main loop - handle system events
    while (1) {
        // System idle - tasks run independently
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Log system status periodically
        ESP_LOGI(TAG, "Heap: %lu bytes", esp_get_free_heap_size());
    }
}
