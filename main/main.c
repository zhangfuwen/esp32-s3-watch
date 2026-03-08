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

static const char *TAG = "WATCH";

// Touch callback
static void touch_handler(touch_event_t event, uint8_t count)
{
    ESP_LOGI(TAG, ">>> TOUCH EVENT: %d, count: %d", event, count);
    
    if (event == TOUCH_EVENT_DOUBLE_TAP) {
        ESP_LOGI(TAG, ">>> DOUBLE TAP - Toggle LVGL interface!");
        // TODO: Toggle LVGL visibility
    } else if (event == TOUCH_EVENT_PRESS) {
        ESP_LOGI(TAG, ">>> TOUCH PRESS - Change display color!");
        // TODO: Change color
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

    // Initialize display with LVGL
    ESP_LOGI(TAG, "Initializing LVGL display...");
    display_init();  // Initialize hardware
    
    esp_err_t lvgl_ret = lvgl_init_system();  // Initialize LVGL
    if (lvgl_ret == ESP_OK) {
        lvgl_start_tasks();  // Start LVGL tasks
        ESP_LOGI(TAG, "LVGL initialized!");
        
        // Create a simple LVGL test UI
        lv_obj_t *label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "ESP32-S3 Watch\nLVGL v0.3.1");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label, DISPLAY_WIDTH - 20);
        
        ESP_LOGI(TAG, "LVGL test UI created");
    } else {
        ESP_LOGE(TAG, "LVGL init failed, display will show red test only");
    }
    
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
