/**
 * CST816 Capacitive Touch Panel Driver Header
 * 
 * @file touch_cst816.h
 * @brief Driver for CST816 capacitive touch IC
 * @version 0.1.0
 * @date 2026-03-09
 */

#ifndef _TOUCH_CST816_H_
#define _TOUCH_CST816_H_

#include "esp_err.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

// I2C configuration
#define TOUCH_I2C_NUM           I2C_NUM_0
#define TOUCH_I2C_FREQ_HZ       400000

// CST816 touch event types
typedef enum {
    CST816_EVENT_PRESS = 0,
    CST816_EVENT_RELEASE,
    CST816_EVENT_MOVE,
    CST816_EVENT_GESTURE
} cst816_event_t;

// Touch point structure
typedef struct {
    uint16_t x;
    uint16_t y;
    bool pressed;
    uint8_t gesture;
} cst816_point_t;

// Touch panel handle
typedef struct {
    bool initialized;
    uint16_t chip_id;
    uint8_t firmware_version;
} cst816_handle_t;

// Touch callback function type
typedef void (*cst816_callback_t)(cst816_event_t event, uint16_t x, uint16_t y);

/**
 * @brief Initialize CST816 touch panel
 * @return ESP_OK on success
 */
esp_err_t cst816_init(void);

/**
 * @brief Read touch coordinates
 * @param point Pointer to touch point structure
 * @return ESP_OK on success
 */
esp_err_t cst816_read_touch(touch_point_t *point);

/**
 * @brief Start touch interrupt task
 * @param callback Callback function for touch events
 * @return ESP_OK on success
 */
esp_err_t cst816_start_task(cst816_callback_t callback);

/**
 * @brief Get touch panel handle
 * @return Pointer to touch panel handle
 */
touch_panel_handle_t* cst816_get_handle(void);

#ifdef __cplusplus
}
#endif

#endif // _TOUCH_CST816_H_
