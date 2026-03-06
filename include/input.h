/**
 * Input Driver Header
 * 
 * @file input.h
 * @brief Button and touch input interface
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

// Button identifiers
#define BUTTON_A    0
#define BUTTON_B    1
#define BUTTON_C    2

// Input event callback type
typedef void (*input_callback_t)(int button_id, void *user_data);

/**
 * @brief Initialize input system
 * @return ESP_OK on success
 */
esp_err_t input_init(void);

/**
 * @brief Check if button is pressed
 * @param button_id Button identifier
 * @return true if pressed
 */
bool input_button_pressed(int button_id);

/**
 * @brief Register button press callback
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void input_register_callback(input_callback_t callback, void *user_data);

#endif // INPUT_H
