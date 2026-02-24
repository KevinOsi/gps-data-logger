#include "telemetry_task.h"
#include "telemetry.h"
#include "gps_parser.h"
#include "hw_config.h"
#include "i2c_manager.h"
#include "bme280_handler.h"
#include "mag_handler.h"
#include "esp_log.h"
#include <string.h>
#include <inttypes.h>

static const char *TAG = "TELEMETRY_TASK";

void telemetry_task(void *pvParameters) {
    uint8_t data[128];
    ubx_nav_pvt_t local_pvt;
    bme280_data_t local_env;
    mag_data_t local_mag;
    uint32_t last_env_poll = 0;
    
    ESP_LOGI(TAG, "Telemetry task started on Core %d", xPortGetCoreID());
    
    gps_parser_init();

    while (1) {
        // 1. Read GPS Data from UART
        int len = uart_read_bytes(GPS_UART_NUM, data, sizeof(data), 20 / portTICK_PERIOD_MS);
        
        if (len > 0) {
            for (int i = 0; i < len; i++) {
                if (gps_parse_byte(data[i], &local_pvt)) {
                    // Valid PVT packet parsed
                    if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
                        memcpy(&g_telemetry.gps, &local_pvt, sizeof(ubx_nav_pvt_t));
                        g_telemetry.last_update_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
                        xSemaphoreGive(g_telemetry_mutex);
                        
                        ESP_LOGD(TAG, "GPS Update: Lat: %" PRIi32 ", Lon: %" PRIi32 ", Fix: %" PRIu8, 
                                 local_pvt.lat, local_pvt.lon, local_pvt.fixType);
                    }
                }
            }
        }
        
        // 2. Poll Sensors every 100ms (10Hz)
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (now - last_env_poll >= 100) {
            bool env_ok = (bme280_handler_read(&local_env) == ESP_OK);
            bool mag_ok = (mag_handler_read(&local_mag) == ESP_OK);
            
            if (xSemaphoreTake(g_telemetry_mutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
                if (env_ok) {
                    memcpy(&g_telemetry.env, &local_env, sizeof(bme280_data_t));
                }
                if (mag_ok) {
                    memcpy(&g_telemetry.mag, &local_mag, sizeof(mag_data_t));
                }
                xSemaphoreGive(g_telemetry_mutex);
            }
            last_env_poll = now;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
    }
}

void telemetry_task_start() {
    // Pin this task to Core 0 (Protocol Core) as per specifications
    xTaskCreatePinnedToCore(telemetry_task, "telemetry_task", 4096, NULL, 10, NULL, 0);
}
