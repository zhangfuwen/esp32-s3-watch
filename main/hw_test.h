/**
 * Hardware Test UI Header
 * 
 * @file hw_test.h
 * @brief LVGL-based hardware testing interface
 * @version 0.4.0
 * @date 2026-03-09
 */

#ifndef HW_TEST_H
#define HW_TEST_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize hardware test UI
 */
void hw_test_init(void);

/**
 * @brief Get hardware test screen
 * @return Pointer to the test screen
 */
lv_obj_t *hw_test_get_screen(void);

#ifdef __cplusplus
}
#endif

#endif // HW_TEST_H
