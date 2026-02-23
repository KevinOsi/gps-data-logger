#include "ui_task.h"
#include "oled_handler.h"
#include "ssd1306.h"
#include "telemetry.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

static const char *TAG = "UI_TASK";
extern SSD1306_t dev; // Defined in oled_handler.c

void ui_task(void *pvParameters) {
    char buf[32];
    ubx_nav_pvt_t local_gps;
    bme280_data_t local_env;
    
    ESP_LOGI(TAG, "UI task started on Core %d", xPortGetCoreID());
    
    oled_handler_init();

    while (1) {
        // 1. Get Telemetry Snapshot
        if (xSemaphoreTake(g_telemetry_mutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
            memcpy(&local_gps, &g_telemetry.gps, sizeof(ubx_nav_pvt_t));
            memcpy(&local_env, &g_telemetry.env, sizeof(bme280_data_t));
            xSemaphoreGive(g_telemetry_mutex);
        }

        // 2. Render UI
        ssd1306_clear_screen(&dev, false);
        
        // Header: Sats, Fix, Accuracy
        snprintf(buf, sizeof(buf), "S:%u F:%u A:%.1f", 
                 local_gps.numSV, local_gps.fixType, local_gps.hAcc / 1000.0f);
        ssd1306_display_text(&dev, 0, buf, strlen(buf), false);
        
        // Body Line 1: Latitude
        snprintf(buf, sizeof(buf), "Lat:%10.6f", local_gps.lat / 10000000.0f);
        ssd1306_display_text(&dev, 2, buf, strlen(buf), false);
        
        // Body Line 2: Longitude
        snprintf(buf, sizeof(buf), "Lon:%10.6f", local_gps.lon / 10000000.0f);
        ssd1306_display_text(&dev, 3, buf, strlen(buf), false);
        
        // Body Line 3: Altitude
        snprintf(buf, sizeof(buf), "Alt:%7.1fm", local_env.altitude);
        ssd1306_display_text(&dev, 5, buf, strlen(buf), false);
        
        // Footer: Speed, Heading
        snprintf(buf, sizeof(buf), "V:%4.1f H:%3" PRIi32, 
                 local_gps.gSpeed / 1000.0f, local_gps.heading / 100000);
        ssd1306_display_text(&dev, 7, buf, strlen(buf), false);

        vTaskDelay(200 / portTICK_PERIOD_MS); // 5Hz Refresh
    }
}

void ui_task_start() {
    xTaskCreatePinnedToCore(ui_task, "ui_task", 4096, NULL, 5, NULL, 1);
}
