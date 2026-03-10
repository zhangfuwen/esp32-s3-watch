/**
 * CST816 Capacitive Touch Panel Driver for ESP32-S3 Watch
 * 
 * @file touch_cst816.c
 * @brief Driver for CST816 capacitive touch IC
 * @version 0.1.0
 * @date 2026-03-09
 */

#include "touch_cst816.h"
#include "board_config.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "CST816";

// CST816 Register addresses
#define CST816_REG_GESTURE      0x01
#define CST816_REG_FINGER_NUM   0x02
#define CST816_REG_XY_COORD     0x03

// Touch state
static cst816_handle_t touch_handle = {0};
static TaskHandle_t touch_task_handle = NULL;
static cst816_callback_t touch_cb = NULL;

// I2C read from CST816
static esp_err_t cst816_read_reg(uint8_t reg, uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TOUCH_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TOUCH_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(TOUCH_I2C_NUM, cmd, pdMS_TO_TICKS(10));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

// Initialize CST816
esp_err_t cst816_init(void)
{
    ESP_LOGI(TAG, "Initializing CST816 touch panel...");
    
    // Configure reset pin
    gpio_config_t rst_conf = {
        .pin_bit_mask = (1ULL << TOUCH_RST_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&rst_conf);
    
    // Configure interrupt pin
    gpio_config_t int_conf = {
        .pin_bit_mask = (1ULL << TOUCH_INT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&int_conf);
    
    // Configure I2C (shared with audio and IMU)
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = TOUCH_I2C_SDA_PIN,
        .scl_io_num = TOUCH_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = TOUCH_I2C_FREQ_HZ,
    };
    
    ESP_LOGI(TAG, "Installing I2C driver...");
    esp_err_t ret = i2c_param_config(TOUCH_I2C_NUM, &i2c_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: 0x%x", ret);
        return ret;
    }
    
    // Try to install I2C driver (may already be installed by other drivers)
    ret = i2c_driver_install(TOUCH_I2C_NUM, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM);
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "I2C driver already installed, continuing...");
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: 0x%x", ret);
        return ret;
    } else {
        ESP_LOGI(TAG, "I2C driver installed");
    }
    
    // Reset CST816
    ESP_LOGI(TAG, "Resetting CST816...");
    gpio_set_level(TOUCH_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(TOUCH_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    ESP_LOGI(TAG, "CST816 reset complete");
    
    // Try to read chip ID (CST816 uses register 0xA7 for chip ID)
    uint8_t chip_id[2];
    ret = cst816_read_reg(0xA7, chip_id, 2);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "CST816 Chip ID: 0x%02x%02x", chip_id[0], chip_id[1]);
        touch_handle.chip_id = (chip_id[0] << 8) | chip_id[1];
        touch_handle.initialized = true;
        ESP_LOGI(TAG, "CST816 initialized successfully");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read CST816 chip ID: 0x%x", ret);
        ESP_LOGW(TAG, "CST816 not detected, touch will not work");
        return ret;
    }
}

// Read touch coordinates
esp_err_t cst816_read_touch(cst816_point_t *point)
{
    if (!touch_handle.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Fixed 2026-03-10: Buffer must be 6 bytes (was 5, causing buffer[5] out-of-bounds access)
    // Register layout: [gesture][finger_num][x_high][x_low][y_high][y_low]
    uint8_t buffer[6];
    esp_err_t ret = cst816_read_reg(CST816_REG_GESTURE, buffer, 6);
    if (ret != ESP_OK) {
        return ret;
    }
    
    uint8_t gesture = buffer[0];
    uint8_t finger_num = buffer[1];
    
    if (finger_num > 0) {
        // Extract coordinates
        uint16_t x = ((buffer[2] & 0x0F) << 8) | buffer[3];
        uint16_t y = ((buffer[4] & 0x0F) << 8) | buffer[5];
        
        point->x = x;
        point->y = y;
        point->pressed = true;
        point->gesture = gesture;
        
        ESP_LOGV(TAG, "Touch: x=%d, y=%d, gesture=%d", x, y, gesture);
    } else {
        point->pressed = false;
        point->x = 0;
        point->y = 0;
        point->gesture = 0;
    }
    
    return ESP_OK;
}

// Touch interrupt handler
static void IRAM_ATTR touch_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // Notify touch task
    if (touch_task_handle) {
        vTaskNotifyGiveFromISR(touch_task_handle, &xHigherPriorityTaskWoken);
    }
    
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Touch task - reads coordinates on interrupt
static void touch_task(void *pvParameters)
{
    cst816_point_t point;
    bool last_pressed = false;
    
    while (1) {
        // Wait for interrupt
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // Debounce
        vTaskDelay(pdMS_TO_TICKS(5));
        
        // Read touch data
        esp_err_t ret = cst816_read_touch(&point);
        if (ret == ESP_OK) {
            // Detect press/release
            if (point.pressed && !last_pressed) {
                // New touch
                if (touch_cb) {
                    touch_cb(CST816_EVENT_PRESS, point.x, point.y);
                }
                ESP_LOGI(TAG, "Touch Press: (%d, %d)", point.x, point.y);
            } else if (!point.pressed && last_pressed) {
                // Release
                if (touch_cb) {
                    touch_cb(CST816_EVENT_RELEASE, point.x, point.y);
                }
                ESP_LOGI(TAG, "Touch Release");
            } else if (point.pressed && last_pressed) {
                // Move
                if (touch_cb) {
                    touch_cb(CST816_EVENT_MOVE, point.x, point.y);
                }
            }
            
            last_pressed = point.pressed;
        }
        
        // Clear interrupt flag (read gesture register)
        uint8_t dummy;
        cst816_read_reg(CST816_REG_GESTURE, &dummy, 1);
    }
}

// Start touch task
esp_err_t cst816_start_task(cst816_callback_t callback)
{
    touch_cb = callback;
    
    // Install I2C interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(TOUCH_INT_PIN, touch_isr_handler, NULL);
    
    // Create touch task
    xTaskCreate(touch_task, "cst816_touch", 4096, NULL, 10, &touch_task_handle);
    
    ESP_LOGI(TAG, "CST816 touch task started");
    
    return ESP_OK;
}

// Get touch handle
cst816_handle_t* cst816_get_handle(void)
{
    return &touch_handle;
}
