#include "ui_task.h"
#include "ssd1306.h"
#include "esp_log.h"
#include "telemetry.h"
#include "button_handler.h"
#include "system_manager.h"
#include "mag_handler.h"
#include "hw_config.h"
#include <math.h>
#include <string.h>

static const char *TAG = "UI_TASK";

typedef enum {
    UI_PAGE_GPS,
    UI_PAGE_ENV,
    UI_PAGE_COMPASS,
    UI_PAGE_OFFLOAD,
    UI_PAGE_MAX
} ui_page_t;

static ui_page_t current_page = UI_PAGE_GPS;

static void draw_needle(SSD1306_t *dev, float heading_deg) {
    int cx = 96, cy = 32, r = 20;
    float rad = (heading_deg - 90) * M_PI / 180.0;
    int x = cx + (int)(r * cos(rad));
    int y = cy + (int)(r * sin(rad));
    
    // Draw cardinal markers in buffer
    for(int i=0; i<360; i+=45) {
        float a = i * M_PI / 180.0;
        _ssd1306_pixel(dev, cx + (int)((r+2)*cos(a)), cy + (int)((r+2)*sin(a)), false);
    }
    _ssd1306_line(dev, cx, cy, x, y, false);
}

void ui_task(void *pvParameters) {
    SSD1306_t dev;
    i2c_device_add(&dev, I2C_MASTER_NUM, -1, I2C_ADDRESS);
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xFF);

    button_event_t btn_evt;
    char buf[32];
    ui_page_t last_page = 0xFF;

    while (1) {
        if (button_handler_get_event(&btn_evt, 0) == ESP_OK) {
            if (btn_evt == BUTTON_EVENT_SINGLE_PRESS) {
                current_page = (current_page + 1) % UI_PAGE_MAX;
            } else if (btn_evt == BUTTON_EVENT_LONG_PRESS) {
                if (current_page == UI_PAGE_OFFLOAD) {
                    system_mode_t next = (system_manager_get_mode() == SYSTEM_MODE_LOGGING) ? SYSTEM_MODE_OFFLOAD : SYSTEM_MODE_LOGGING;
                    system_manager_set_mode(next);
                } else if (current_page == UI_PAGE_COMPASS) {
                    mag_handler_set_offset(0); // This should be updated to 'capture current as zero'
                    ESP_LOGI(TAG, "Compass offset reset requested");
                }
            }
        }

        // If page changed, clear physical screen once
        if (current_page != last_page) {
            ssd1306_clear_screen(&dev, false);
            last_page = current_page;
        }

        global_telemetry_t snap;
        if (xSemaphoreTake(g_telemetry_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            memcpy(&snap, &g_telemetry, sizeof(global_telemetry_t));
            xSemaphoreGive(g_telemetry_mutex);

            switch (current_page) {
                case UI_PAGE_GPS:
                    ssd1306_display_text(&dev, 0, "--- GPS ---", 11, false);
                    snprintf(buf, sizeof(buf), "Fix: %d Sats: %d", (int)snap.gps.fixType, (int)snap.gps.numSV);
                    ssd1306_clear_line(&dev, 2, false);
                    ssd1306_display_text(&dev, 2, buf, strlen(buf), false);
                    
                    snprintf(buf, sizeof(buf), "Lat:%ld", (long)snap.gps.lat);
                    ssd1306_clear_line(&dev, 4, false);
                    ssd1306_display_text(&dev, 4, buf, strlen(buf), false);
                    
                    snprintf(buf, sizeof(buf), "Lon:%ld", (long)snap.gps.lon);
                    ssd1306_clear_line(&dev, 5, false);
                    ssd1306_display_text(&dev, 5, buf, strlen(buf), false);
                    break;

                case UI_PAGE_ENV:
                    ssd1306_display_text(&dev, 0, "--- ENV ---", 11, false);
                    snprintf(buf, sizeof(buf), "Temp: %.1f C", snap.env.temperature);
                    ssd1306_clear_line(&dev, 2, false);
                    ssd1306_display_text(&dev, 2, buf, strlen(buf), false);
                    
                    snprintf(buf, sizeof(buf), "Pres: %.1f hPa", snap.env.pressure);
                    ssd1306_clear_line(&dev, 4, false);
                    ssd1306_display_text(&dev, 4, buf, strlen(buf), false);
                    
                    snprintf(buf, sizeof(buf), "Alt: %.1fm", snap.fused_alt);
                    ssd1306_clear_line(&dev, 6, false);
                    ssd1306_display_text(&dev, 6, buf, strlen(buf), false);
                    break;

                case UI_PAGE_COMPASS:
                    ssd1306_display_text(&dev, 0, "COMPASS", 7, false);
                    snprintf(buf, sizeof(buf), "H: %.1f*", snap.mag.heading);
                    ssd1306_clear_line(&dev, 2, false);
                    ssd1306_display_text(&dev, 2, buf, strlen(buf), false);
                    
                    // Clear needle area by clearing specific lines
                    ssd1306_clear_line(&dev, 3, false);
                    ssd1306_clear_line(&dev, 4, false);
                    ssd1306_clear_line(&dev, 5, false);
                    draw_needle(&dev, snap.mag.heading);
                    break;

                case UI_PAGE_OFFLOAD:
                    ssd1306_display_text(&dev, 0, "OFFLOAD", 7, false);
                    bool is_ap = (snap.mode == SYSTEM_MODE_OFFLOAD);
                    ssd1306_clear_line(&dev, 2, false);
                    ssd1306_display_text(&dev, 2, is_ap ? "Wi-Fi: ON" : "Wi-Fi: OFF", 10, false);
                    ssd1306_display_text(&dev, 4, "Hold Button", 11, false);
                    break;

                default: break;
            }
        }

        ssd1306_show_buffer(&dev); 
        vTaskDelay(pdMS_TO_TICKS(100)); // Back to 10Hz
    }
}

void ui_task_start() {
    xTaskCreatePinnedToCore(ui_task, "ui_task", 4096, NULL, 5, NULL, 1);
}
