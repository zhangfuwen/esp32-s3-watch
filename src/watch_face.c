/**
 * Watch Face Implementation
 * 
 * @file watch_face.c
 * @brief Watch face UI rendering and time management
 */

#include "watch_face.h"
#include "display.h"
#include "board_config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>

static const char *TAG = "WATCH_FACE";

static TaskHandle_t watch_face_task_handle = NULL;
static bool show_seconds = true;
static int current_style = 0;

/**
 * @brief Get current time
 */
static void get_current_time(struct tm *timeinfo)
{
    time_t now;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    now = tv.tv_sec;
    localtime_r(&now, timeinfo);
}

/**
 * @brief Draw a simple analog clock face
 */
static void draw_analog_clock(struct tm *timeinfo)
{
    int center_x = DISPLAY_WIDTH / 2;
    int center_y = DISPLAY_HEIGHT / 2 - 20;
    int radius = 80;
    
    // Calculate hand angles
    float hour_angle = (timeinfo->tm_hour % 12) * 30 + timeinfo->tm_min * 0.5;
    float min_angle = timeinfo->tm_min * 6;
    float sec_angle = timeinfo->tm_sec * 6;
    
    // Draw clock face circle
    display_draw_circle(center_x, center_y, radius, 0xFFFF);
    
    // TODO: Draw hour markers
    // TODO: Draw hour hand
    // TODO: Draw minute hand
    // TODO: Draw second hand
}

/**
 * @brief Watch face main task
 */
static void watch_face_task(void *pvParameters)
{
    struct tm timeinfo;
    int frame_count = 0;
    
    ESP_LOGI(TAG, "Watch face task started");
    ESP_LOGI(TAG, "Display: %dx%d", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    // Show boot message
    display_clear();
    display_draw_text(20, DISPLAY_HEIGHT / 2 - 10, "ESP32-S3 Watch");
    display_draw_text(40, DISPLAY_HEIGHT / 2 + 20, "v0.1.0");
    display_update();
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    while (1) {
        // Get current time
        get_current_time(&timeinfo);
        
        // Clear and redraw
        display_clear();
        
        // Draw digital time (format: HH:MM or HH:MM:SS)
        char time_str[32];
        if (show_seconds) {
            snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
                     timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        } else {
            snprintf(time_str, sizeof(time_str), "%02d:%02d", 
                     timeinfo.tm_hour, timeinfo.tm_min);
        }
        
        // Center time text
        int text_width = strlen(time_str) * 16; // Approximate for larger font
        int x = (DISPLAY_WIDTH - text_width) / 2;
        int y = DISPLAY_HEIGHT / 2 - 10;
        
        display_draw_text(x, y, time_str);
        
        // Draw date
        char date_str[32];
        snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d %s",
                 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                 timeinfo.tm_wday == 0 ? "Sun" : 
                 timeinfo.tm_wday == 1 ? "Mon" :
                 timeinfo.tm_wday == 2 ? "Tue" :
                 timeinfo.tm_wday == 3 ? "Wed" :
                 timeinfo.tm_wday == 4 ? "Thu" :
                 timeinfo.tm_wday == 5 ? "Fri" : "Sat");
        text_width = strlen(date_str) * 10;
        x = (DISPLAY_WIDTH - text_width) / 2;
        display_draw_text(x, y + 35, date_str);
        
        // Alternate: Draw analog clock every other frame
        if (frame_count % 2 == 0) {
            draw_analog_clock(&timeinfo);
        }
        
        // Update display
        display_update();
        
        frame_count++;
        
        // Update every second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void watch_face_start(void)
{
    if (watch_face_task_handle == NULL) {
        xTaskCreate(watch_face_task, "watch_face", 4096, NULL, 5, &watch_face_task_handle);
        ESP_LOGI(TAG, "Watch face task created");
    }
}

void watch_face_stop(void)
{
    if (watch_face_task_handle != NULL) {
        vTaskDelete(watch_face_task_handle);
        watch_face_task_handle = NULL;
        ESP_LOGI(TAG, "Watch face task stopped");
    }
}

void watch_face_update(void)
{
    // Trigger immediate update
}

void watch_face_set_style(int style_id)
{
    current_style = style_id;
    ESP_LOGI(TAG, "Watch face style changed: %d", style_id);
}

void watch_face_show_seconds(bool show)
{
    show_seconds = show;
    ESP_LOGI(TAG, "Show seconds: %s", show ? "yes" : "no");
}
