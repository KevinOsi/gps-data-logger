#include "telemetry_task.h"
#include "telemetry.h"
#include "gps_parser.h"
#include "hw_config.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "TELEMETRY_TASK";

void telemetry_task(void *pvParameters) {
    uint8_t data[128];
    ubx_nav_pvt_t local_pvt;
    
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
                        
                        ESP_LOGD(TAG, "GPS Update: Lat: %d, Lon: %d, Fix: %d", 
                                 local_pvt.lat, local_pvt.lon, local_pvt.fixType);
                    }
                }
            }
        }
        
        // 2. Poll BME280 and Compass (To be implemented in future tracks)
        // We will use the shared I2C bus here with a mutex if necessary.

        vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
    }
}

void telemetry_task_start() {
    // Pin this task to Core 0 (Protocol Core) as per specifications
    xTaskCreatePinnedToCore(telemetry_task, "telemetry_task", 4096, NULL, 10, NULL, 0);
}
