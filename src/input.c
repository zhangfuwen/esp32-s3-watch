/**
 * Input Driver Implementation
 * 
 * @file input.c
 * @brief Button and touch input handling
 * 
 * Hardware: espwatch-s3b2
 * - BOOT Button: GPIO 0
 * - POWER Button: GPIO 45
 * - Volume Up/Down: Not connected
 */

#include "input.h"
#include "board_config.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "INPUT";

// Button configuration table
typedef struct {
    int button_id;
    gpio_num_t gpio_pin;
    const char *name;
} button_config_t;

static button_config_t buttons[] = {
    {BUTTON_BOOT,   BOOT_BUTTON_GPIO,   "BOOT"},
    {BUTTON_POWER,  POWER_BUTTON_GPIO,  "POWER"},
    {-1, GPIO_NUM_NC, NULL}  // Sentinel
};

static input_callback_t g_callback = NULL;
static void *g_user_data = NULL;

/**
 * @brief GPIO interrupt handler
 */
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    
    // Defer processing to main task
    if (g_callback != NULL) {
        // Find button ID
        for (int i = 0; buttons[i].button_id >= 0; i++) {
            if (buttons[i].gpio_pin == gpio_num) {
                g_callback(buttons[i].button_id, g_user_data);
                break;
            }
        }
    }
}

esp_err_t input_init(void)
{
    ESP_LOGI(TAG, "Initializing input buttons");
    
    esp_err_t ret;
    
    // Create GPIO interrupt queue
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM);
    
    // Configure each button
    for (int i = 0; buttons[i].button_id >= 0; i++) {
        if (buttons[i].gpio_pin == GPIO_NUM_NC) {
            ESP_LOGD(TAG, "Skipping %s (not connected)", buttons[i].name);
            continue;
        }
        
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << buttons[i].gpio_pin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_ANYEDGE,
        };
        
        ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure %s", buttons[i].name);
            continue;
        }
        
        // Add ISR handler
        ret = gpio_isr_handler_add(buttons[i].gpio_pin, gpio_isr_handler, 
                                   (void *) buttons[i].gpio_pin);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to add ISR for %s", buttons[i].name);
        }
        
        ESP_LOGI(TAG, "Button %s initialized (GPIO %d)", 
                 buttons[i].name, buttons[i].gpio_pin);
    }
    
    ESP_LOGI(TAG, "Input initialized");
    return ESP_OK;
}

bool input_button_pressed(int button_id)
{
    // TODO: Read GPIO state
    return false;
}

void input_register_callback(input_callback_t callback, void *user_data)
{
    // TODO: Register interrupt handler
    ESP_LOGD(TAG, "Input callback registered");
}
