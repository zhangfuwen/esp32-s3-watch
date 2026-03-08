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
    ESP_LOGI(TAG, "Calling display_init()...");
    display_init();
    ESP_LOGI(TAG, "display_init() returned - starting color cycle test!");
    
    // Color cycle test loop - changes color every 2 seconds
    // This helps diagnose display issues even if you connect late
    uint16_t test_colors[] = {
        0xF800,  // Red
        0x07E0,  // Green
        0x001F,  // Blue
        0xFFFF,  // White
        0x0000,  // Black
    };
    const char* color_names[] = {"RED", "GREEN", "BLUE", "WHITE", "BLACK"};
    
    ESP_LOGI(TAG, "Starting color cycle test loop...");
    
    // Initialize input system (buttons/touch)
    ESP_LOGI(TAG, "Initializing input...");
    input_init();
    
    // Initialize WiFi
    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init();
    
    // Initialize Bluetooth
    ESP_LOGI(TAG, "Initializing Bluetooth...");
    bluetooth_init();
    
    // Start watch face task - COMMENTED OUT for debugging
    // ESP_LOGI(TAG, "Starting watch face...");
    // watch_face_start();
    
    ESP_LOGI(TAG, "=== System Ready ===");
    ESP_LOGI(TAG, "Display init complete - keeping RED screen for testing");
    ESP_LOGI(TAG, "Screen should show SOLID RED from display_init()");
    
    // Keep display_init state - don't change colors
    // Commented out color cycle for debugging
    /*
    int color_index = 0;
    while (1) {
        display_fill(test_colors[color_index]);
        vTaskDelay(pdMS_TO_TICKS(3000));
        color_index = (color_index + 1) % 5;
    }
    */
    
    // Just idle forever to keep display_init state
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
