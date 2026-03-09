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
#include "lvgl_port.h"
#include "lvgl.h"
#include "touch.h"
#include "hw_test.h"

static const char *TAG = "WATCH";

// LVGL state
static bool lvgl_initialized = false;
static bool lvgl_visible = false;

// Forward declaration
static void start_lvgl_ui(void);

// Touch callback
static void touch_handler(touch_event_t event, uint8_t count)
{
    ESP_LOGI(TAG, "=================================");
    ESP_LOGI(TAG, ">>> TOUCH EVENT: %d, count: %d", event, count);
    ESP_LOGI(TAG, "=================================");
    
    if (event == TOUCH_EVENT_SINGLE_TAP) {
        ESP_LOGI(TAG, ">>> SINGLE TAP DETECTED!");
        if (!lvgl_initialized) {
            ESP_LOGI(TAG, ">>> Starting LVGL for the first time...");
            start_lvgl_ui();
        } else {
            ESP_LOGI(TAG, ">>> LVGL already running!");
        }
    } else if (event == TOUCH_EVENT_DOUBLE_TAP) {
        ESP_LOGI(TAG, ">>> DOUBLE TAP DETECTED!");
        // TODO: Toggle visibility or other action
    } else if (event == TOUCH_EVENT_PRESS) {
        ESP_LOGI(TAG, ">>> TOUCH PRESS (count=%d)", count);
    }
}

// Start LVGL UI
static void start_lvgl_ui(void)
{
    ESP_LOGI(TAG, "=== LVGL UI Start ===");
    
    if (lvgl_initialized) {
        ESP_LOGW(TAG, "LVGL already initialized!");
        return;
    }
    
    // Initialize LVGL system
    ESP_LOGI(TAG, "Calling lvgl_init_system()...");
    esp_err_t ret = lvgl_init_system();
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "✓ lvgl_init_system() SUCCESS!");
        lvgl_initialized = true;
        
        // Start LVGL task
        ESP_LOGI(TAG, "Calling lvgl_start_tasks()...");
        lvgl_start_tasks();
        ESP_LOGI(TAG, "✓ LVGL tasks started!");
        
        // Create UI elements
        ESP_LOGI(TAG, "Creating LVGL label...");
        lv_obj_t *label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "ESP32-S3 Watch\nLVGL v0.3.1\nDouble tap to toggle!");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label, DISPLAY_WIDTH - 20);
        ESP_LOGI(TAG, "✓ LVGL label created!");
        
        lvgl_visible = true;
        ESP_LOGI(TAG, "=== LVGL UI Ready ===");
    } else {
        ESP_LOGE(TAG, "✗ lvgl_init_system() FAILED! Error: 0x%x", ret);
        ESP_LOGE(TAG, "Display will show red test only");
    }
}

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

    // Display will be initialized by LVGL when BOOT button is pressed
    ESP_LOGI(TAG, "Display will be initialized by LVGL on first touch");
    ESP_LOGI(TAG, "Press BOOT button to start LVGL");
    
    // Initialize touch
    ESP_LOGI(TAG, "Initializing touch...");
    touch_init();
    touch_start_task(touch_handler);
    
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
    ESP_LOGI(TAG, "Touch the BOOT button to test!");
    ESP_LOGI(TAG, "Double tap to toggle LVGL interface");
    
    // Keep alive forever
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
