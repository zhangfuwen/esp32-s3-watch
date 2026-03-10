/**
 * ESP32-S3 Watch Board Configuration
 * 
 * @file board_config.h
 * @brief Hardware pin definitions and board-specific configuration
 * @version 0.1
 * @date 2026-03-06
 * 
 * Board: espwatch-s3b2
 */

#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

//==================== Audio Configuration ====================
#define AUDIO_INPUT_SAMPLE_RATE     24000
#define AUDIO_OUTPUT_SAMPLE_RATE    24000
#define AUDIO_INPUT_REFERENCE       true

// I2S Audio Pins
#define AUDIO_I2S_GPIO_MCLK         GPIO_NUM_21
#define AUDIO_I2S_GPIO_WS           GPIO_NUM_14
#define AUDIO_I2S_GPIO_BCLK         GPIO_NUM_18
#define AUDIO_I2S_GPIO_DIN          GPIO_NUM_17
#define AUDIO_I2S_GPIO_DOUT         GPIO_NUM_13

// Audio Codec (ES8311 with PCA9557)
#define AUDIO_CODEC_USE_PCA9557
#define AUDIO_CODEC_PA_PIN          GPIO_NUM_48
#define AUDIO_CODEC_I2C_SDA_PIN     GPIO_NUM_1
#define AUDIO_CODEC_I2C_SCL_PIN     GPIO_NUM_2
#define AUDIO_CODEC_ES8311_ADDR     ES8311_CODEC_DEFAULT_ADDR

//==================== Buttons & Input ====================
#define BUILTIN_LED_GPIO            GPIO_NUM_NC       // Not connected
#define BOOT_BUTTON_GPIO            GPIO_NUM_0        // Boot button
#define POWER_BUTTON_GPIO           GPIO_NUM_35       // Power button
#define VOLUME_UP_BUTTON_GPIO       GPIO_NUM_NC       // Not connected
#define VOLUME_DOWN_BUTTON_GPIO     GPIO_NUM_NC       // Not connected

//==================== Touch Panel (CST816) ====================
#define TOUCH_I2C_SDA_PIN           GPIO_NUM_1        // Shared I2C SDA
#define TOUCH_I2C_SCL_PIN           GPIO_NUM_2        // Shared I2C SCL
#define TOUCH_RST_PIN               GPIO_NUM_7        // TP_RST
#define TOUCH_INT_PIN               GPIO_NUM_46       // TP_INT
#define TOUCH_I2C_ADDR              (0x15)            // CST816 default address

//==================== Display Configuration ====================
// Display uses I2C shared with audio codec
#define DISPLAY_SDA_PIN             AUDIO_CODEC_I2C_SDA_PIN   // GPIO 1
#define DISPLAY_SCL_PIN             AUDIO_CODEC_I2C_SCL_PIN   // GPIO 2

// Display SPI pins (for LCD)
// #define LCD_TYPE_JD9853_SERIAL    // Uncomment if using JD9853 serial LCD
#define DISPLAY_MISO_PIN            GPIO_NUM_NC       // Not connected
#define DISPLAY_MOSI_PIN            GPIO_NUM_6
#define DISPLAY_SCLK_PIN            GPIO_NUM_5
#define DISPLAY_CS_PIN              GPIO_NUM_12
#define DISPLAY_DC_PIN              GPIO_NUM_11
#define DISPLAY_RESET_PIN           GPIO_NUM_NC       // Not connected
#define DISPLAY_BACKLIGHT_PIN       GPIO_NUM_47  // Backlight control

// Vibration Motor
#define VIB_MOTOR_PIN               GPIO_NUM_4

//==================== Display Parameters ====================
#define DISPLAY_WIDTH               240
#define DISPLAY_HEIGHT              285

// Display orientation and color settings
#define DISPLAY_SWAP_XY             false
#define DISPLAY_MIRROR_X            false
#define DISPLAY_MIRROR_Y            false
#define DISPLAY_INVERT_COLOR        false
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

// Display offset - ST7789 controller defaults to 240x320, need to center 240x285
// Fixed 2026-03-10: Set Y offset to 0 to display from top (user reported 20+ pixel downward shift)
#define DISPLAY_OFFSET_X            0
#define DISPLAY_OFFSET_Y            0  // Changed from 17 - display from top edge

//==================== Touch Panel Parameters ====================
// CST816 touch panel resolution
// Fixed 2026-03-10 v0.5.8: User reported max Y=284, so use 285 as divisor
// This means CST816 and display are 1:1 matched (240x285)
#define TOUCH_CST816_MAX_X          240   // CST816 X resolution (0-239 = 240 values)
#define TOUCH_CST816_MAX_Y          285   // CST816 Y resolution (0-284 = 285 values) - MATCHES DISPLAY!

// Touch calibration mode - set to 1 to enable detailed logging
#define TOUCH_CALIBRATION_MODE      1

//==================== I2C Configuration ====================
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0

//==================== Power Management ====================
// TODO: Add ADC pin for battery voltage monitoring
// TODO: Add charging status pin

#endif // _BOARD_CONFIG_H_
