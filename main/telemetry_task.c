#include "telemetry_task.h"
#include "telemetry.h"
#include "gps_parser.h"
#include "hw_config.h"
#include "i2c_manager.h"
#include "bme280_handler.h"
#include "mag_handler.h"
#include "sd_card_handler.h"
#include "logger_task.h"
#include "ble_manager.h"
#include "esp_log.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

static const char *TAG = "TELEMETRY_TASK";

void telemetry_task(void *pvParameters) {
    uint8_t data[128];
    ubx_nav_pvt_t local_pvt;
    bme280_data_t local_env;
    mag_data_t local_mag;
    uint32_t last_env_poll = 0;
    uint32_t last_sd_poll = 0;
    uint32_t last_ble_push = 0;
    
    ESP_LOGI(TAG, "Telemetry task started on Core %d", xPortGetCoreID());
    
    gps_parser_init();

    while (1) {
        // 1. Read GPS Data from UART
        int len = uart_read_bytes(GPS_UART_NUM, data, sizeof(data), 20 / portTICK_PERIOD_MS);
        
        if (len > 0) {
            for (int i = 0; i < len; i++) {
                if (gps_parse_byte(data[i], &local_pvt)) {
                    if (xSemaphoreTake(g_telemetry_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
                        memcpy(&g_telemetry.gps, &local_pvt, sizeof(ubx_nav_pvt_t));
                        g_telemetry.last_update_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
                        xSemaphoreGive(g_telemetry_mutex);
                    }
                }
            }
        }
        
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        uint32_t poll_interval = (g_telemetry.mode == SYSTEM_MODE_LOGGING) ? 100 : 1000;

        // 2. Poll Sensors
        if (now - last_env_poll >= poll_interval) {
            bool env_ok = (bme280_handler_read(&local_env) == ESP_OK);
            bool mag_ok = (mag_handler_read(&local_mag) == ESP_OK);
            
            if (xSemaphoreTake(g_telemetry_mutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
                if (env_ok) memcpy(&g_telemetry.env, &local_env, sizeof(bme280_data_t));
                if (mag_ok) memcpy(&g_telemetry.mag, &local_mag, sizeof(mag_data_t));
                
                // 3. Altitude Fusion
                if (!g_telemetry.alt_calibrated && g_telemetry.gps.fixType >= 3 && g_telemetry.gps.vAcc < 10000) {
                    float gps_alt = g_telemetry.gps.hMSL / 1000.0f;
                    float p_measured = g_telemetry.env.pressure;
                    if (p_measured > 800.0f) {
                        float qnh = p_measured / powf(1.0f - (gps_alt / 44330.0f), 1.0f / 0.1902949f);
                        bme280_handler_set_qnh(qnh);
                        g_telemetry.alt_calibrated = true;
                        
                        // Also calculate dynamic declination once fix is obtained
                        float decl = mag_handler_calculate_declination(g_telemetry.gps.lat / 1e7, g_telemetry.gps.lon / 1e7);
                        mag_handler_set_offset_direct(mag_handler_get_offset() + decl);
                        
                        logger_log_system_event("CALIBRATED GPS+ALT+MAG");
                    }
                }

                // 4. Save config every 5 minutes if logging
                static uint32_t last_config_save = 0;
                if (now - last_config_save > 300000) {
                    extern esp_err_t config_save(float mag, float qnh);
                    config_save(mag_handler_get_offset(), bme280_handler_get_qnh());
                    last_config_save = now;
                }
                g_telemetry.fused_alt = g_telemetry.env.altitude;

                if (g_telemetry.mode == SYSTEM_MODE_LOGGING) {
                    logger_enqueue(&g_telemetry);
                }
                
                if (g_telemetry.mode == SYSTEM_MODE_LOGGING && now - last_ble_push >= 500) {
                    ble_manager_update_telemetry(&g_telemetry);
                    last_ble_push = now;
                }
                g_telemetry.poi_pressed = false;
                xSemaphoreGive(g_telemetry_mutex);
            }
            last_env_poll = now;
        }

        // 3. Poll SD Space
        if (now - last_sd_poll >= 30000) {
            uint32_t total, free;
            if (sd_card_get_info(&total, &free) == ESP_OK) {
                if (xSemaphoreTake(g_telemetry_mutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
                    g_telemetry.sd_total_mb = total;
                    g_telemetry.sd_free_mb = free;
                    xSemaphoreGive(g_telemetry_mutex);
                }
            }
            last_sd_poll = now;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void telemetry_task_start() {
    xTaskCreatePinnedToCore(telemetry_task, "telemetry_task", 4096, NULL, 5, NULL, 0);
}
