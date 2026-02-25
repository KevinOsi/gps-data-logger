#include "logger_task.h"
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
#define LOG_FILE_PATH MOUNT_POINT "/log.csv"

void logger_task(void *pvParameters) {
    global_telemetry_t snapshot;
    FILE *f = NULL;
    
    ESP_LOGI(TAG, "Logger task started on Core %d", xPortGetCoreID());

    // 1. Open Log File
    f = fopen(LOG_FILE_PATH, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open log file for writing");
        // We continue anyway to drain the queue, but we won't log.
    } else {
        // 2. Write CSV Header if file is new/empty
        fseek(f, 0, SEEK_END);
        if (ftell(f) == 0) {
            fprintf(f, "Timestamp,Lat,Lon,Alt,Sats,Fix,Temp,Press,Hum,MagH\n");
        }
    }

    uint32_t last_sync = 0;

    while (1) {
        if (xQueueReceive(logger_queue, &snapshot, portMAX_DELAY) == pdTRUE) {
            if (f != NULL) {
                // 3. Serialize Row
                fprintf(f, "%u,%ld,%ld,%.2f,%u,%u,%.2f,%.2f,%.2f,%.1f\n",
                        snapshot.last_update_ms,
                        snapshot.gps.lat, snapshot.gps.lon, snapshot.env.altitude,
                        snapshot.gps.numSV, snapshot.gps.fixType,
                        snapshot.env.temperature, snapshot.env.pressure, snapshot.env.humidity,
                        snapshot.mag.heading);
                
                // 4. Periodic Sync (every 5 seconds) to prevent data loss
                uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
                if (now - last_sync > 5000) {
                    fflush(f);
                    fsync(fileno(f));
                    last_sync = now;
                    ESP_LOGD(TAG, "Log file synced to SD");
                }
            }
        }
    }

    if (f != NULL) fclose(f);
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
        ESP_LOGW(TAG, "Logger queue full!");
        return ESP_FAIL;
    }

    return ESP_OK;
}
