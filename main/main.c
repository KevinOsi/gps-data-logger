#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hw_config.h"
#include "telemetry.h"
#include "telemetry_task.h"
#include "ui_task.h"
#include "logger_task.h"
#include "bme280_handler.h"
#include "mag_handler.h"
#include "oled_handler.h"
#include "sd_card_handler.h"
#include "button_handler.h"
#include "ble_manager.h"
#include "gps_config.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP32 GPS Logger...");

    // 0. Initialize NVS (Required for BLE and Wi-Fi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 0.1 Initialize Network Interface and Event Loop (Required for Wi-Fi)
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 1. Initialize Hardware (Basic GPIO/UART/I2C Master)
    if (hw_config_init() != ESP_OK) {
        ESP_LOGE(TAG, "Hardware initialization failed. System halted.");
        return;
    }

    // 1.1 Initialize Button Handler
    button_handler_init(POI_BUTTON_PIN);

    // 1.2 Initialize BLE
    if (ble_manager_init() != ESP_OK) {
        ESP_LOGE(TAG, "BLE initialization failed.");
    }

    // 1.3 Apply GPS performance configuration
    gps_config_apply();

    g_telemetry.mode = SYSTEM_MODE_LOGGING;

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

    // 3. Mount SD Card
    if (sd_card_mount() == ESP_OK) {
        // Load persistent calibration
        extern esp_err_t config_load();
        config_load();

        logger_task_start();
        
        // Log Reset Reason
        char reset_msg[64];
        esp_reset_reason_t reason = esp_reset_reason();
        snprintf(reset_msg, sizeof(reset_msg), "SYSTEM BOOT. Reason: %d", reason);
        logger_log_system_event(reset_msg);
    } else {
        ESP_LOGE(TAG, "SD Card mount failed. Logging will be disabled.");
    }

    // 4. Start Background Tasks
    telemetry_task_start();
    ui_task_start();

    uint32_t last_heap_log = 0;

    // 5. Main Loop
    while (1) {
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
            uint32_t free_heap = esp_get_free_heap_size();
            uint32_t min_free_heap = esp_get_minimum_free_heap_size();
            
            ESP_LOGI(TAG, "Lat:%ld Lon:%ld Env:%.1fhPa %.1fC Mag:%.1f* Heap:%" PRIu32 " (Min:%" PRIu32 ")",
                     g_telemetry.gps.lat, g_telemetry.gps.lon,
                     g_telemetry.env.pressure, g_telemetry.env.temperature,
                     g_telemetry.mag.heading, free_heap, min_free_heap);
            
            // Log heap status to SD every 60 seconds
            if (now - last_heap_log >= 60000) {
                char heap_msg[64];
                snprintf(heap_msg, sizeof(heap_msg), "MEM STAT: Free:%" PRIu32 " Min:%" PRIu32, free_heap, min_free_heap);
                logger_log_system_event(heap_msg);
                last_heap_log = now;
            }

            xSemaphoreGive(g_telemetry_mutex);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Log every 5 seconds
    }
}
