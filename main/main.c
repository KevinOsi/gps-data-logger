#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hw_config.h"
#include "telemetry.h"
#include "telemetry_task.h"
#include "ui_task.h"
#include "bme280_handler.h"
#include "mag_handler.h"
#include "oled_handler.h"
#include "sd_card_handler.h"
#include "driver/i2c.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP32 GPS Logger...");

    // 1. Initialize Hardware (Basic GPIO/UART/I2C Master)
    if (hw_config_init() != ESP_OK) {
        ESP_LOGE(TAG, "Hardware initialization failed. System halted.");
        return;
    }

    // 2. Sequential Sensor Initialization (No contention)
    ESP_LOGI(TAG, "Initializing Sensors...");
    
    if (bme280_handler_init() != ESP_OK) {
        ESP_LOGE(TAG, "BME280 init failed");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (mag_handler_init() != ESP_OK) {
        ESP_LOGE(TAG, "Magnetometer init failed");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

    oled_handler_init();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // 3. Mount SD Card
    if (sd_card_mount() == ESP_OK) {
        logger_task_start();
    } else {
        ESP_LOGE(TAG, "SD Card mount failed. Logging will be disabled.");
    }

    // 4. Start Background Tasks
    telemetry_task_start();
    ui_task_start();

    // 5. Main Loop
    while (1) {
        if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
            ESP_LOGI(TAG, "TOCK: Lat:%ld Lon:%ld Env:%.1fhPa %.1fC Mag:%.1f*",
                     g_telemetry.gps.lat, g_telemetry.gps.lon,
                     g_telemetry.env.pressure, g_telemetry.env.temperature,
                     g_telemetry.mag.heading);
            xSemaphoreGive(g_telemetry_mutex);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 1Hz "Tick Tock"
    }
}
