#include "ui_task.h"
#include "oled_handler.h"
#include "ssd1306.h"
#include "telemetry.h"
#include "i2c_manager.h"
#include "button_handler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

static const char *TAG = "UI_TASK";
extern SSD1306_t dev; // Defined in oled_handler.c

typedef enum {
    UI_PAGE_NAV,
    UI_PAGE_ENV,
    UI_PAGE_STATUS,
    UI_PAGE_MAX
} ui_page_t;

static ui_page_t current_page = UI_PAGE_NAV;

static void render_nav_page(const global_telemetry_t *tele) {
    char buf[32];
    bool blink = (tele->last_update_ms % 1000) < 500;
    
    // Header: REC indicator, Sats, Fix
    snprintf(buf, sizeof(buf), "%s S:%-2u F:%u    ", 
             blink ? "REC" : "   ", tele->gps.numSV, tele->gps.fixType);
    ssd1306_display_text(&dev, 0, buf, 16, false);
    
    // Body Line 1: Latitude
    snprintf(buf, sizeof(buf), "Lat:%11.6f ", tele->gps.lat / 10000000.0f);
    ssd1306_display_text(&dev, 2, buf, 16, false);
    
    // Body Line 2: Longitude
    snprintf(buf, sizeof(buf), "Lon:%11.6f ", tele->gps.lon / 10000000.0f);
    ssd1306_display_text(&dev, 3, buf, 16, false);
    
    // Body Line 3: Altitude & Heading
    snprintf(buf, sizeof(buf), "A:%5.0fm C:%3.0f*  ", 
             tele->env.altitude, tele->mag.heading);
    ssd1306_display_text(&dev, 5, buf, 16, false);
    
    // Footer: Speed, Heading (GPS)
    snprintf(buf, sizeof(buf), "V:%4.1f H:%-3" PRIi32 "    ", 
             tele->gps.gSpeed / 1000.0f, tele->gps.heading / 100000);
    ssd1306_display_text(&dev, 7, buf, 16, false);
}

static void render_env_page(const global_telemetry_t *tele) {
    char buf[32];
    snprintf(buf, sizeof(buf), "ENVIRONMENTAL    ");
    ssd1306_display_text(&dev, 0, buf, 16, false);

    snprintf(buf, sizeof(buf), "Temp: %5.1f C   ", tele->env.temperature);
    ssd1306_display_text(&dev, 2, buf, 16, false);

    snprintf(buf, sizeof(buf), "Press: %6.1f hPa", tele->env.pressure);
    ssd1306_display_text(&dev, 3, buf, 16, false);

    snprintf(buf, sizeof(buf), "Humid: %5.1f %%   ", tele->env.humidity);
    ssd1306_display_text(&dev, 4, buf, 16, false);

    snprintf(buf, sizeof(buf), "Alt:   %5.0f m   ", tele->env.altitude);
    ssd1306_display_text(&dev, 5, buf, 16, false);

    snprintf(buf, sizeof(buf), "Heading: %3.0f*  ", tele->mag.heading);
    ssd1306_display_text(&dev, 7, buf, 16, false);
}

static void render_status_page(const global_telemetry_t *tele) {
    char buf[32];
    snprintf(buf, sizeof(buf), "SYSTEM STATUS    ");
    ssd1306_display_text(&dev, 0, buf, 16, false);

    snprintf(buf, sizeof(buf), "Satellites: %u   ", tele->gps.numSV);
    ssd1306_display_text(&dev, 2, buf, 16, false);

    snprintf(buf, sizeof(buf), "SD Free: %" PRIu32 " MB  ", tele->sd_free_mb);
    ssd1306_display_text(&dev, 3, buf, 16, false);

    snprintf(buf, sizeof(buf), "SD Total: %" PRIu32 " MB ", tele->sd_total_mb);
    ssd1306_display_text(&dev, 4, buf, 16, false);

    uint32_t uptime_sec = tele->last_update_ms / 1000;
    snprintf(buf, sizeof(buf), "Uptime: %02" PRIu32 ":%02" PRIu32 ":%02" PRIu32, 
             (uptime_sec / 3600), (uptime_sec / 60) % 60, uptime_sec % 60);
    ssd1306_display_text(&dev, 6, buf, 16, false);
}

void ui_task(void *pvParameters) {
    global_telemetry_t local_tele;
    button_event_t btn_evt;
    
    ESP_LOGI(TAG, "UI task started on Core %d", xPortGetCoreID());
    
    while (1) {
        // 1. Handle Button Events
        while (button_handler_get_event(&btn_evt, 0) == ESP_OK) {
            if (btn_evt == BUTTON_EVENT_SINGLE_PRESS) {
                current_page = (current_page + 1) % UI_PAGE_MAX;
                ESP_LOGI(TAG, "Page cycled to %d", current_page);
                ssd1306_clear_screen(&dev, false);
            } else if (btn_evt == BUTTON_EVENT_DOUBLE_PRESS) {
                ESP_LOGI(TAG, "POI Mark Requested!");
                if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
                    g_telemetry.poi_pressed = true;
                    xSemaphoreGive(g_telemetry_mutex);
                }
            }
        }

        // 2. Get Telemetry Snapshot
        if (xSemaphoreTake(g_telemetry_mutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
            memcpy(&local_tele, &g_telemetry, sizeof(global_telemetry_t));
            xSemaphoreGive(g_telemetry_mutex);
        }

        // 3. Render Current Page
        switch (current_page) {
            case UI_PAGE_NAV:
                render_nav_page(&local_tele);
                break;
            case UI_PAGE_ENV:
                render_env_page(&local_tele);
                break;
            case UI_PAGE_STATUS:
                render_status_page(&local_tele);
                break;
            default:
                break;
        }

        vTaskDelay(200 / portTICK_PERIOD_MS); // 5Hz Refresh
    }
}

void ui_task_start() {
    xTaskCreatePinnedToCore(ui_task, "ui_task", 4096, NULL, 5, NULL, 1);
}
