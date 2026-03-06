/**
 * Watch Face Implementation
 * 
 * @file watch_face.c
 * @brief Watch face UI rendering and time management
 */

#include "watch_face.h"
#include "display.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>
#include <sys/time.h>

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
 * @brief Watch face main task
 */
static void watch_face_task(void *pvParameters)
{
    struct tm timeinfo;
    
    ESP_LOGI(TAG, "Watch face task started");
    
    while (1) {
        // Get current time
        get_current_time(&timeinfo);
        
        // Clear and redraw
        display_clear();
        
        // Draw time (format: HH:MM or HH:MM:SS)
        char time_str[32];
        if (show_seconds) {
            snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
                     timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        } else {
            snprintf(time_str, sizeof(time_str), "%02d:%02d", 
                     timeinfo.tm_hour, timeinfo.tm_min);
        }
        
        // Center text
        int text_width = strlen(time_str) * 12; // Approximate
        int x = (DISPLAY_WIDTH - text_width) / 2;
        int y = DISPLAY_HEIGHT / 2;
        
        display_draw_text(x, y, time_str);
        
        // Draw date
        char date_str[32];
        snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
                 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
        text_width = strlen(date_str) * 8;
        x = (DISPLAY_WIDTH - text_width) / 2;
        display_draw_text(x, y + 30, date_str);
        
        // Update display
        display_update();
        
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
