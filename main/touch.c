/**
 * Touch/Input Button Driver for ESP32-S3 Watch
 * 
 * @file touch.c
 * @brief Button input detection using GPIO (BOOT button as touch substitute)
 * @version 0.3.1
 * @date 2026-03-08
 */

#include "touch.h"
#include "board_config.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "TOUCH";

// Touch state
static bool touch_detected = false;
static uint32_t last_touch_time = 0;
static uint8_t touch_count = 0;
static touch_callback_t touch_cb = NULL;

// Initialize touch pad (using BOOT button GPIO 0)
esp_err_t touch_init(void)
{
    ESP_LOGI(TAG, "Initializing touch button (GPIO 0)...");
    
    // Configure GPIO 0 as input with pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BOOT_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "Touch button initialized");
    
    return ESP_OK;
}

// Read touch value (simulated - returns 0 when pressed, 1000 when not)
uint16_t touch_read(void)
{
    int level = gpio_get_level(BOOT_BUTTON_GPIO);
    return level ? 1000 : 0;  // 0 = pressed, 1000 = not pressed
}

// Check if touch detected (button pressed)
bool touch_is_pressed(void)
{
    int level = gpio_get_level(BOOT_BUTTON_GPIO);
    return level == 0;  // Active low (pressed = 0)
}

// Touch detection task
static void touch_detect_task(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(50);  // Check every 50ms
    
    while (1) {
        uint32_t current_time = xTaskGetTickCount();
        
        if (touch_is_pressed()) {
            if (!touch_detected) {
                // New touch detected
                touch_detected = true;
                touch_count++;
                last_touch_time = current_time;
                
                ESP_LOGI(TAG, "Touch detected! Count: %d", touch_count);
                
                // Call callback if registered
                if (touch_cb) {
                    touch_cb(TOUCH_EVENT_PRESS, touch_count);
                }
            }
        } else {
            if (touch_detected) {
                // Touch released
                touch_detected = false;
                
                // Check for double tap (within 300ms)
                if (touch_count == 2 && (current_time - last_touch_time) < pdMS_TO_TICKS(300)) {
                    ESP_LOGI(TAG, "Double tap detected!");
                    if (touch_cb) {
                        touch_cb(TOUCH_EVENT_DOUBLE_TAP, 0);
                    }
                    touch_count = 0;
                } else if (touch_count == 1) {
                    // Single tap
                    ESP_LOGI(TAG, "Single tap");
                    if (touch_cb) {
                        touch_cb(TOUCH_EVENT_SINGLE_TAP, 0);
                    }
                    touch_count = 0;
                }
                
                // Reset if too long since last touch
                if ((current_time - last_touch_time) > pdMS_TO_TICKS(300)) {
                    touch_count = 0;
                }
            }
        }
        
        vTaskDelay(xDelay);
    }
}

// Start touch detection task
esp_err_t touch_start_task(touch_callback_t callback)
{
    touch_cb = callback;
    
    // Increased stack size to 4096 to prevent overflow
    xTaskCreate(touch_detect_task, "touch_detect", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Touch detection task started");
    
    return ESP_OK;
}
