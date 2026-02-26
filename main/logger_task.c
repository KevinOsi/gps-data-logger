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
static QueueHandle_t event_queue = NULL;

#define LOGGER_QUEUE_LEN 20
#define EVENT_QUEUE_LEN 10
#define MOUNT_POINT "/sd"
#define SYSTEM_LOG_FILE MOUNT_POINT "/SYSTEM.LOG"

typedef struct {
    char message[64];
} system_event_t;

void logger_task(void *pvParameters) {
    global_telemetry_t snapshot;
    system_event_t event;
    FILE *f_csv = NULL;
    FILE *f_gpx = NULL;
    FILE *f_sys = NULL;
    char filename[32];
    bool open_attempted = false;
    uint32_t count = 0;
    
    ESP_LOGI(TAG, "Logger task started on Core %d", xPortGetCoreID());

    uint32_t last_sync = 0;

    while (1) {
        // 1. Check for system events (priority)
        if (xQueueReceive(event_queue, &event, 0) == pdTRUE) {
            f_sys = fopen(SYSTEM_LOG_FILE, "a");
            if (f_sys) {
                fprintf(f_sys, "[%" PRIu32 "] %s\n", (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS), event.message);
                fclose(f_sys);
                ESP_LOGI(TAG, "System Event Logged: %s", event.message);
            }
        }

        // 2. Check for telemetry snapshots
        if (xQueueReceive(logger_queue, &snapshot, pdMS_TO_TICKS(100)) == pdTRUE) {
            // 1. Open files only when a valid 3D fix is acquired
            if (!open_attempted) {
                if (snapshot.gps.fixType >= 3 && snapshot.gps.year >= 2025) {
                    open_attempted = true;
                    
                    // Format: LMMDDHH.CSV (e.g., L022512.CSV)
                    snprintf(filename, sizeof(filename), MOUNT_POINT "/L%02u%02u%02u.CSV",
                             snapshot.gps.month, snapshot.gps.day, snapshot.gps.hour);
                    
                    f_csv = fopen(filename, "w");
                    if (f_csv) {
                        fprintf(f_csv, "Timestamp,Lat,Lon,Alt,Sats,Fix,Temp,Press,Hum,MagH,POI\n");
                        ESP_LOGI(TAG, "Created CSV log: %s", filename);
                    }

                    snprintf(filename, sizeof(filename), MOUNT_POINT "/L%02u%02u%02u.GPX",
                             snapshot.gps.month, snapshot.gps.day, snapshot.gps.hour);
                    
                    f_gpx = fopen(filename, "w");
                    if (f_gpx) {
                        fprintf(f_gpx, "%s", gpx_get_header());
                        ESP_LOGI(TAG, "Created GPX log: %s", filename);
                    }
                    
                    logger_log_system_event("LOG FILES OPENED");
                }
            }

            // 2. Logging Logic (only if files are open)
            if (open_attempted && (f_csv != NULL || f_gpx != NULL)) {
                if (f_csv != NULL) {
                    fprintf(f_csv, "%" PRIu32 ",%ld,%ld,%.2f,%u,%u,%.2f,%.2f,%.2f,%.1f,%d\n",
                            snapshot.last_update_ms,
                            snapshot.gps.lat, snapshot.gps.lon, snapshot.fused_alt,
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
                }
            }
        }
    }
}

void logger_task_start(void) {
    logger_queue = xQueueCreate(LOGGER_QUEUE_LEN, sizeof(global_telemetry_t));
    event_queue = xQueueCreate(EVENT_QUEUE_LEN, sizeof(system_event_t));
    
    if (logger_queue == NULL || event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create logger queues");
        return;
    }

    xTaskCreatePinnedToCore(logger_task, "logger_task", 4096, NULL, 2, NULL, 1);
}

esp_err_t logger_enqueue(const global_telemetry_t *snapshot) {
    if (logger_queue == NULL) return ESP_FAIL;
    if (xQueueSend(logger_queue, snapshot, 0) != pdTRUE) return ESP_FAIL;
    return ESP_OK;
}

void logger_log_system_event(const char *event) {
    if (event_queue == NULL) return;
    system_event_t evt;
    strncpy(evt.message, event, sizeof(evt.message) - 1);
    evt.message[sizeof(evt.message) - 1] = '\0';
    xQueueSend(event_queue, &evt, 0);
}
