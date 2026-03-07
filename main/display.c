/**
 * Display Driver Implementation
 * 
 * @file display.c
 * @brief Display driver for ESP32-S3 Watch
 * 
 * TODO: Update based on actual display hardware:
 * - SPI LCD (ST7789, ILI9341, etc.)
 * - I2C OLED (SSD1306, SH1106)
 * - MIPI-DSI display
 */

#include <string.h>
#include "display.h"
#include "board_config.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

static const char *TAG = "DISPLAY";

// Display buffer (RGB565 format)
static uint16_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

// Forward declarations for static functions
static void st7789_write_cmd(uint8_t cmd);
static void st7789_write_data(const uint8_t *data, int len);
static void st7789_init(void);
static void display_gpio_init(void);

// SPI handle
static spi_device_handle_t spi_handle;

/**
 * @brief Initialize I2C bus (shared with audio codec)
 */
static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = DISPLAY_SDA_PIN,
        .scl_io_num = DISPLAY_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    i2c_param_config(I2C_MASTER_NUM, &conf);
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 
                              I2C_MASTER_RX_BUF_DISABLE, 
                              I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief Initialize GPIO for display control
 */
static void display_gpio_init(void)
{
    // Configure DC pin (command/data select)
    gpio_reset_pin(DISPLAY_DC_PIN);
    gpio_set_direction(DISPLAY_DC_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DISPLAY_DC_PIN, 0);
    
    // Configure backlight pin
    gpio_reset_pin(DISPLAY_BACKLIGHT_PIN);
    gpio_set_direction(DISPLAY_BACKLIGHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DISPLAY_BACKLIGHT_PIN, 0);
    
    // NOTE: CS pin is managed by SPI driver, don't configure it manually
    
    ESP_LOGI(TAG, "Display GPIO initialized (DC=%d, BL=%d)", DISPLAY_DC_PIN, DISPLAY_BACKLIGHT_PIN);
}

/**
 * @brief Send command to ST7789 display
 */
static void st7789_write_cmd(uint8_t cmd)
{
    gpio_set_level(DISPLAY_DC_PIN, 0);  // Command mode
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
        .flags = SPI_TRANS_USE_TXDATA
    };
    spi_device_transmit(spi_handle, &t);
}

/**
 * @brief Send data to ST7789 display
 */
static void st7789_write_data(const uint8_t *data, int len)
{
    gpio_set_level(DISPLAY_DC_PIN, 1);  // Data mode
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data
    };
    spi_device_transmit(spi_handle, &t);
}

/**
 * @brief Initialize ST7789 display controller
 */
static void st7789_init(void)
{
    ESP_LOGI(TAG, "Initializing ST7789 display controller...");
    
    // Initialize GPIO
    display_gpio_init();
    
    // Turn on backlight first to see if it's working
    ESP_LOGI(TAG, "Turning on backlight (GPIO %d)", DISPLAY_BACKLIGHT_PIN);
    gpio_set_level(DISPLAY_BACKLIGHT_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Software reset
    ESP_LOGI(TAG, "Sending software reset (0x01)");
    st7789_write_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(150));
    
    // Exit sleep mode
    ESP_LOGI(TAG, "Exiting sleep mode (0x11)");
    st7789_write_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(150));
    
    // Memory Data Access Control - BGR format, normal orientation
    // 0x00 = RGB, 0x08 = BGR
    ESP_LOGI(TAG, "Setting MADCTL (0x36) - BGR format");
    uint8_t madctl = 0x08;  // BGR color format
    st7789_write_cmd(0x36);
    st7789_write_data(&madctl, 1);
    
    // Pixel format: 16-bit/pixel (RGB565)
    ESP_LOGI(TAG, "Setting pixel format (0x3A)");
    uint8_t colmod = 0x55;
    st7789_write_cmd(0x3A);
    st7789_write_data(&colmod, 1);
    
    // Set column address (240 width)
    ESP_LOGI(TAG, "Setting column address (0x2A)");
    uint8_t caset[] = {0x00, 0x00, 0x00, 0xEF};  // 0-239
    st7789_write_cmd(0x2A);
    st7789_write_data(caset, 4);
    
    // Set row address (284 height)
    ESP_LOGI(TAG, "Setting row address (0x2B)");
    uint8_t raset[] = {0x00, 0x00, 0x01, 0x1B};  // 0-283
    st7789_write_cmd(0x2B);
    st7789_write_data(raset, 4);
    
    // Display inversion ON (common for ST7789)
    ESP_LOGI(TAG, "Enabling display inversion (0x21)");
    st7789_write_cmd(0x21);
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Turn on display
    ESP_LOGI(TAG, "Turning on display (0x29)");
    st7789_write_cmd(0x29);
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Fill screen with colors to test
    ESP_LOGI(TAG, "Testing with WHITE screen first");
    display_fill(0xFFFF);  // White
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    ESP_LOGI(TAG, "ST7789 initialization complete");
}

/**
 * @brief Initialize SPI bus for display
 */
static esp_err_t spi_master_init(void)
{
    esp_err_t ret;
    
    // Initialize GPIO first
    display_gpio_init();
    
    // SPI bus configuration
    spi_bus_config_t buscfg = {
        .miso_io_num = DISPLAY_MISO_PIN,
        .mosi_io_num = DISPLAY_MOSI_PIN,
        .sclk_io_num = DISPLAY_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2,
    };
    
    // SPI device configuration - Try mode 0 (some ST7789 variants)
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20 * 1000 * 1000,  // 20 MHz
        .mode = 0,  // Try SPI mode 0 (CPOL=0, CPHA=0)
        .spics_io_num = DISPLAY_CS_PIN,
        .queue_size = 1,  // Single transaction queue to avoid conflicts
        .flags = 0,
    };
    
    // Initialize SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus");
        return ret;
    }
    
    // Add device to SPI bus
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device");
        spi_bus_free(SPI2_HOST);
        return ret;
    }
    
    return ESP_OK;
}

/**
 * @brief Initialize backlight PWM
 */
static void backlight_init(void)
{
    // TODO: Configure LEDC PWM for backlight
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DISPLAY_BACKLIGHT_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
    };
    gpio_config(&io_conf);
    gpio_set_level(DISPLAY_BACKLIGHT_PIN, 0);
}

esp_err_t display_init(void)
{
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Initializing display (SPI, %dx%d)", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    ESP_LOGI(TAG, "========================================");
    
    esp_err_t ret;
    
    // Initialize I2C (shared with audio codec)
    ESP_LOGI(TAG, "Step 1: Initializing I2C...");
    ret = i2c_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "FAILED: I2C init (error %d)", ret);
        return ret;
    }
    ESP_LOGI(TAG, "SUCCESS: I2C initialized");
    
    // Initialize SPI
    ESP_LOGI(TAG, "Step 2: Initializing SPI...");
    ret = spi_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "FAILED: SPI init (error %d)", ret);
        return ret;
    }
    ESP_LOGI(TAG, "SUCCESS: SPI initialized");
    
    // Initialize backlight
    ESP_LOGI(TAG, "Step 3: Initializing backlight...");
    backlight_init();
    ESP_LOGI(TAG, "SUCCESS: Backlight initialized");
    
    // Initialize display controller
    ESP_LOGI(TAG, "Step 4: Initializing ST7789 controller...");
    st7789_init();
    ESP_LOGI(TAG, "SUCCESS: ST7789 initialized - screen should be RED now!");
    
    // DON'T clear - leave red for debugging!
    // memset(display_buffer, 0x00, sizeof(display_buffer));
    // display_update();
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Display initialization COMPLETE!");
    ESP_LOGI(TAG, "Starting color cycle test...");
    ESP_LOGI(TAG, "========================================");
    return ESP_OK;
}

void display_clear(void)
{
    memset(display_buffer, 0x00, sizeof(display_buffer));
}

void display_draw_text(int x, int y, const char *text)
{
    // TODO: Implement text rendering with font
    ESP_LOGD(TAG, "Draw text at (%d, %d): %s", x, y, text);
}

void display_draw_pixel(int x, int y, uint16_t color)
{
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
        return;
    }
    display_buffer[y * DISPLAY_WIDTH + x] = color;
}

void display_draw_rect(int x, int y, int width, int height, uint16_t color)
{
    // TODO: Implement rectangle drawing
}

void display_draw_circle(int x, int y, int radius, uint16_t color)
{
    // TODO: Implement circle drawing (Bresenham's algorithm)
}

/**
 * @brief Fill screen with a solid color
 */
void display_fill(uint16_t color)
{
    ESP_LOGI(TAG, "Filling screen with color 0x%04X", color);
    
    // Set column address
    uint8_t caset[] = {0x00, 0x00, 0x00, 0xEF};  // 0-239
    st7789_write_cmd(0x2A);
    st7789_write_data(caset, 4);
    
    // Set row address
    uint8_t raset[] = {0x00, 0x00, 0x01, 0x1B};  // 0-283
    st7789_write_cmd(0x2B);
    st7789_write_data(raset, 4);
    
    // Write RAM command
    st7789_write_cmd(0x2C);
    
    // Fill buffer with color
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        display_buffer[i] = color;
    }
    
    // Send in chunks to avoid SPI max transfer size limit
    // ESP32-S3 SPI max is ~64KB, screen is 136KB, so send 2 lines at a time
    const int LINES_PER_CHUNK = 8;
    const int CHUNK_SIZE = DISPLAY_WIDTH * LINES_PER_CHUNK;
    
    for (int chunk = 0; chunk < DISPLAY_HEIGHT; chunk += LINES_PER_CHUNK) {
        spi_transaction_t t = {
            .length = CHUNK_SIZE * 16,
            .tx_buffer = &display_buffer[chunk * DISPLAY_WIDTH],
        };
        spi_device_transmit(spi_handle, &t);
    }
    
    ESP_LOGI(TAG, "Screen filled with color 0x%04X", color);
}

void display_update(void)
{
    ESP_LOGD(TAG, "Updating display");
    
    // Set column address
    uint8_t caset[] = {0x00, 0x00, 0x00, 0xEF};  // 0-239
    st7789_write_cmd(0x2A);
    st7789_write_data(caset, 4);
    
    // Set row address
    uint8_t raset[] = {0x00, 0x00, 0x01, 0x1B};  // 0-283
    st7789_write_cmd(0x2B);
    st7789_write_data(raset, 4);
    
    // Write RAM command
    st7789_write_cmd(0x2C);
    
    // Send in chunks to avoid SPI max transfer size limit
    const int LINES_PER_CHUNK = 8;
    const int CHUNK_SIZE = DISPLAY_WIDTH * LINES_PER_CHUNK;
    
    for (int chunk = 0; chunk < DISPLAY_HEIGHT; chunk += LINES_PER_CHUNK) {
        spi_transaction_t t = {
            .length = CHUNK_SIZE * 16,
            .tx_buffer = &display_buffer[chunk * DISPLAY_WIDTH],
        };
        spi_device_transmit(spi_handle, &t);
    }
}

void display_set_brightness(uint8_t brightness)
{
    // TODO: Implement PWM brightness control
    ESP_LOGD(TAG, "Brightness: %d", brightness);
}

void display_sleep(void)
{
    // TODO: Enter display sleep mode
    ESP_LOGI(TAG, "Display sleep");
}

void display_wake(void)
{
    // TODO: Wake display from sleep
    ESP_LOGI(TAG, "Display wake");
}
