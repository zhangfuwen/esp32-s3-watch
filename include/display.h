/**
 * Display Driver Header
 * 
 * @file display.h
 * @brief Abstract display interface for ESP32-S3 Watch
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

// Display dimensions are defined in board_config.h
// Include board_config.h for actual pin definitions and display parameters
#include "board_config.h"

/**
 * @brief Initialize display hardware
 * @return ESP_OK on success
 */
esp_err_t display_init(void);

/**
 * @brief Clear display buffer
 */
void display_clear(void);

/**
 * @brief Draw text at specified position
 * @param x X coordinate (pixels)
 * @param y Y coordinate (pixels)
 * @param text String to display
 */
void display_draw_text(int x, int y, const char *text);

/**
 * @brief Draw pixel at specified position
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Pixel color (RGB565)
 */
void display_draw_pixel(int x, int y, uint16_t color);

/**
 * @brief Draw filled rectangle
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 * @param color Fill color
 */
void display_draw_rect(int x, int y, int width, int height, uint16_t color);

/**
 * @brief Draw circle
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Circle radius
 * @param color Line color
 */
void display_draw_circle(int x, int y, int radius, uint16_t color);

/**
 * @brief Update display with buffer contents
 */
void display_update(void);

/**
 * @brief Set display brightness
 * @param brightness Brightness level (0-255)
 */
void display_set_brightness(uint8_t brightness);

/**
 * @brief Enter sleep mode (display off)
 */
void display_sleep(void);

/**
 * @brief Wake from sleep mode
 */
void display_wake(void);

#endif // DISPLAY_H
