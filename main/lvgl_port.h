/**
 * LVGL Porting Layer Header
 * 
 * @file lvgl_port.h
 * @brief LVGL driver interface
 * @version 0.3.0
 * @date 2026-03-08
 */

#ifndef _LVGL_PORT_H_
#define _LVGL_PORT_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize display driver for LVGL
 * @return ESP_OK on success
 */
esp_err_t lvgl_display_init(void);

/**
 * @brief Initialize LVGL system
 * @return ESP_OK on success
 */
esp_err_t lvgl_init_system(void);

/**
 * @brief Start LVGL background tasks
 * @return ESP_OK on success
 */
esp_err_t lvgl_start_tasks(void);

/**
 * @brief Initialize touch driver
 * @return ESP_OK on success
 */
esp_err_t lvgl_touch_init(void);

#ifdef __cplusplus
}
#endif

#endif // _LVGL_PORT_H_
