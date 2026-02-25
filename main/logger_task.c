#include "logger_task.h"
#include "gpx_formatter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static const char *TAG = "LOGGER_TASK";
static QueueHandle_t logger_queue = NULL;

#define LOGGER_QUEUE_LEN 20
#define MOUNT_POINT "/sd"

void logger_task(void *pvParameters) {
    global_telemetry_t snapshot;
    FILE *f_csv = NULL;
    FILE *f_gpx = NULL;
    char filename[64];
    bool files_opened = false;
    
    ESP_LOGI(TAG, "Logger task started on Core %d", xPortGetCoreID());

    uint32_t last_sync = 0;

    while (1) {
        if (xQueueReceive(logger_queue, &snapshot, portMAX_DELAY) == pdTRUE) {
            // 1. Wait for valid time to open files
            if (!files_opened && snapshot.gps.year >= 2025) {
                // Generate CSV filename
                snprintf(filename, sizeof(filename), MOUNT_POINT "/LOG_%04u%02u%02u_%02u%02u.csv",
                         snapshot.gps.year, snapshot.gps.month, snapshot.gps.day,
                         snapshot.gps.hour, snapshot.gps.minute);
                
                f_csv = fopen(filename, "w");
                if (f_csv) {
                    fprintf(f_csv, "Timestamp,Lat,Lon,Alt,Sats,Fix,Temp,Press,Hum,MagH\n");
                }

                // Generate GPX filename
                snprintf(filename, sizeof(filename), MOUNT_POINT "/LOG_%04u%02u%02u_%02u%02u.gpx",
                         snapshot.gps.year, snapshot.gps.month, snapshot.gps.day,
                         snapshot.gps.hour, snapshot.gps.minute);
                
                f_gpx = fopen(filename, "w");
                if (f_gpx) {
                    fprintf(f_gpx, "%s", gpx_get_header());
                }

                if (f_csv || f_gpx) {
                    files_opened = true;
                    ESP_LOGI(TAG, "Log files opened with timestamp: %04u-%02u-%02u", 
                             snapshot.gps.year, snapshot.gps.month, snapshot.gps.day);
                }
            }

            if (files_opened) {
                // 2. Log CSV
                if (f_csv != NULL) {
                    fprintf(f_csv, "%u,%ld,%ld,%.2f,%u,%u,%.2f,%.2f,%.2f,%.1f\n",
                            snapshot.last_update_ms,
                            snapshot.gps.lat, snapshot.gps.lon, snapshot.env.altitude,
                            snapshot.gps.numSV, snapshot.gps.fixType,
                            snapshot.env.temperature, snapshot.env.pressure, snapshot.env.humidity,
                            snapshot.mag.heading);
                }

                // 3. Log GPX
                if (f_gpx != NULL) {
                    char gpx_buf[512];
                    size_t len = gpx_format_point(gpx_buf, sizeof(gpx_buf), &snapshot);
                    if (len > 0) {
                        fprintf(f_gpx, "%s", gpx_buf);
                    }
                }
                
                // 4. Periodic Sync
                uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
                if (now - last_sync > 5000) {
                    if (f_csv) fflush(f_csv);
                    if (f_gpx) fflush(f_gpx);
                    last_sync = now;
                }
            }
        }
    }

    if (f_csv != NULL) fclose(f_csv);
    if (f_gpx != NULL) {
        fprintf(f_gpx, "%s", gpx_get_footer());
        fclose(f_gpx);
    }
}

void logger_task_start(void) {
    logger_queue = xQueueCreate(LOGGER_QUEUE_LEN, sizeof(global_telemetry_t));
    if (logger_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create logger queue");
        return;
    }

    xTaskCreatePinnedToCore(logger_task, "logger_task", 4096, NULL, 2, NULL, 1);
}

esp_err_t logger_enqueue(const global_telemetry_t *snapshot) {
    if (logger_queue == NULL) return ESP_FAIL;

    if (xQueueSend(logger_queue, snapshot, 0) != pdTRUE) {
        // Queue full, drop snapshot to avoid blocking telemetry
        return ESP_FAIL;
    }

    return ESP_OK;
}
