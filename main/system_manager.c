#include "system_manager.h"
#include "ble_manager.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "logger_task.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "SYSTEM_MANAGER";

static system_mode_t g_current_mode = SYSTEM_MODE_LOGGING;

esp_err_t system_manager_set_mode(system_mode_t mode) {
    if (g_current_mode == mode) return ESP_OK;

    ESP_LOGI(TAG, "Switching system mode from %d to %d...", g_current_mode, mode);
    char mode_msg[64];
    snprintf(mode_msg, sizeof(mode_msg), "MODE SWITCH: %d -> %d", g_current_mode, mode);
    logger_log_system_event(mode_msg);

    if (mode == SYSTEM_MODE_OFFLOAD) {
        // 1. Stop BLE
        ESP_LOGI(TAG, "Stopping BLE to free memory for Wi-Fi...");
        // NimBLE stop isn't always clean, but we'll try to at least stop advertising.
        // For a full stop we'd need to deinit, but NimBLE doesn't easily deinit.
        // We'll just stop advertising for now.
        ble_manager_set_advertising(false);

        // 2. Start Wi-Fi SoftAP
        if (wifi_manager_init_softap() != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start Wi-Fi SoftAP.");
            return ESP_FAIL;
        }

        // 3. Start Web Server
        if (web_server_start() != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start Web Server.");
            wifi_manager_stop();
            return ESP_FAIL;
        }
    } else {
        // Switch back to LOGGING
        // 1. Stop Web Server
        web_server_stop();

        // 2. Stop Wi-Fi
        wifi_manager_stop();

        // 3. Restart BLE
        ble_manager_set_advertising(true);
    }

    g_current_mode = mode;
    
    // Update global telemetry with the new mode and status
    if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
        g_telemetry.mode = g_current_mode;
        g_telemetry.wifi_active = (g_current_mode == SYSTEM_MODE_OFFLOAD);
        xSemaphoreGive(g_telemetry_mutex);
    }

    return ESP_OK;
}

system_mode_t system_manager_get_mode(void) {
    return g_current_mode;
}
