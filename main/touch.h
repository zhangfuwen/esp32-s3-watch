/**
 * Touch Input Driver Header
 * 
 * @file touch.h
 * @brief Touch event types and callbacks
 * @version 0.3.1
 * @date 2026-03-08
 */

#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Touch event types
typedef enum {
    TOUCH_EVENT_PRESS,
    TOUCH_EVENT_SINGLE_TAP,
    TOUCH_EVENT_DOUBLE_TAP
} touch_event_t;

// Touch callback function type
typedef void (*touch_callback_t)(touch_event_t event, uint8_t count);

/**
 * @brief Initialize touch pad
 * @return ESP_OK on success
 */
esp_err_t touch_init(void);

/**
 * @brief Read touch value
 * @return Touch value (lower = more touch)
 */
uint16_t touch_read(void);

/**
 * @brief Check if touch is detected
 * @return true if touched
 */
bool touch_is_pressed(void);

/**
 * @brief Start touch detection task
 * @param callback Function to call on touch events
 * @return ESP_OK on success
 */
esp_err_t touch_start_task(touch_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // _TOUCH_H_
