#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hw_config.h"
#include "telemetry.h"
#include "telemetry_task.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP32 GPS Logger...");

    // 1. Initialize Hardware
    if (hw_config_init() != ESP_OK) {
        ESP_LOGE(TAG, "Hardware initialization failed. System halted.");
        return;
    }

    // 2. Start Telemetry Task (Core 0)
    telemetry_task_start();

    // 3. Main Loop (Core 1 - default)
    while (1) {
        if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
            ESP_LOGI(TAG, "Telemetry Snapshot: Lat: %" PRIi32 ", Lon: %" PRIi32 ", Sats: %" PRIu8 ", Fix: %" PRIu8,
                     g_telemetry.gps.lat, g_telemetry.gps.lon, 
                     g_telemetry.gps.numSV, g_telemetry.gps.fixType);
            ESP_LOGI(TAG, "Env: Temp: %.2f C, Press: %.2f hPa, Alt: %.2f m",
                     g_telemetry.env.temperature, g_telemetry.env.pressure, g_telemetry.env.altitude);
            xSemaphoreGive(g_telemetry_mutex);
        }
        
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Log every 5 seconds
    }
}
