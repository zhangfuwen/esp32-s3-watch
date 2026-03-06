/**
 * Watch Face Header
 * 
 * @file watch_face.h
 * @brief Watch face UI management
 */

#ifndef WATCH_FACE_H
#define WATCH_FACE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Start watch face task
 */
void watch_face_start(void);

/**
 * @brief Stop watch face task
 */
void watch_face_stop(void);

/**
 * @brief Update watch face display
 */
void watch_face_update(void);

/**
 * @brief Set watch face style
 * @param style_id Style identifier
 */
void watch_face_set_style(int style_id);

/**
 * @brief Toggle display of seconds
 * @param show true to show seconds
 */
void watch_face_show_seconds(bool show);

#endif // WATCH_FACE_H
