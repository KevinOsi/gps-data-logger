#include "logger_task.h"
#include "gpx_formatter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>

static const char *TAG = "LOGGER_TASK";
static QueueHandle_t logger_queue = NULL;

#define LOGGER_QUEUE_LEN 20
#define MOUNT_POINT "/sd"

void logger_task(void *pvParameters) {
    global_telemetry_t snapshot;
    FILE *f_csv = NULL;
    FILE *f_gpx = NULL;
    char filename[32];
    bool open_attempted = false;
    uint32_t count = 0;
    
    ESP_LOGI(TAG, "Logger task started on Core %d", xPortGetCoreID());

    uint32_t last_sync = 0;

    while (1) {
        if (xQueueReceive(logger_queue, &snapshot, portMAX_DELAY) == pdTRUE) {
            // 1. Open files on first snapshot
            if (!open_attempted) {
                open_attempted = true;
                
                // Determine CSV filename (Must be 8.3 compliant: max 8 chars before dot)
                if (snapshot.gps.year >= 2025) {
                    // Format: LMMDDHH.CSV (7 chars)
                    snprintf(filename, sizeof(filename), MOUNT_POINT "/L%02u%02u%02u.CSV",
                             snapshot.gps.month, snapshot.gps.day, snapshot.gps.hour);
                } else {
                    snprintf(filename, sizeof(filename), MOUNT_POINT "/NOFIX.CSV");
                }
                
                f_csv = fopen(filename, "w");
                if (f_csv) {
                    fprintf(f_csv, "Timestamp,Lat,Lon,Alt,Sats,Fix,Temp,Press,Hum,MagH,POI\n");
                    ESP_LOGI(TAG, "Created CSV log: %s", filename);
                } else {
                    ESP_LOGE(TAG, "Failed to create CSV log: %s (errno: %d, %s)", 
                             filename, errno, strerror(errno));
                }

                // Determine GPX filename
                if (snapshot.gps.year >= 2025) {
                    // Format: LMMDDHH.GPX
                    snprintf(filename, sizeof(filename), MOUNT_POINT "/L%02u%02u%02u.GPX",
                             snapshot.gps.month, snapshot.gps.day, snapshot.gps.hour);
                } else {
                    snprintf(filename, sizeof(filename), MOUNT_POINT "/NOFIX.GPX");
                }
                
                f_gpx = fopen(filename, "w");
                if (f_gpx) {
                    fprintf(f_gpx, "%s", gpx_get_header());
                    ESP_LOGI(TAG, "Created GPX log: %s", filename);
                } else {
                    ESP_LOGE(TAG, "Failed to create GPX log: %s (errno: %d, %s)", 
                             filename, errno, strerror(errno));
                }
            }

            // 2. Logging Logic
            if (f_csv != NULL || f_gpx != NULL) {
                if (f_csv != NULL) {
                    fprintf(f_csv, "%" PRIu32 ",%ld,%ld,%.2f,%u,%u,%.2f,%.2f,%.2f,%.1f,%d\n",
                            snapshot.last_update_ms,
                            snapshot.gps.lat, snapshot.gps.lon, snapshot.env.altitude,
                            snapshot.gps.numSV, snapshot.gps.fixType,
                            snapshot.env.temperature, snapshot.env.pressure, snapshot.env.humidity,
                            snapshot.mag.heading, snapshot.poi_pressed);
                }

                if (f_gpx != NULL) {
                    char gpx_buf[512];
                    size_t len = gpx_format_point(gpx_buf, sizeof(gpx_buf), &snapshot);
                    if (len > 0) {
                        fprintf(f_gpx, "%s", gpx_buf);
                    }
                }
                
                count++;
                if (count % 100 == 0) {
                    ESP_LOGI(TAG, "Logged %" PRIu32 " points...", count);
                }

                // 3. Periodic Sync (every 5 seconds)
                uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
                if (now - last_sync > 5000) {
                    if (f_csv) {
                        fflush(f_csv);
                        fsync(fileno(f_csv));
                    }
                    if (f_gpx) {
                        fflush(f_gpx);
                        fsync(fileno(f_gpx));
                    }
                    last_sync = now;
                    ESP_LOGD(TAG, "Synced files to SD");
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
