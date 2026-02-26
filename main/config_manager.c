#include "esp_log.h"
#include "esp_err.h"
#include <stdio.h>
#include <string.h>
#include "mag_handler.h"
#include "bme280_handler.h"

static const char *TAG = "CONFIG_MGR";
#define CONFIG_PATH "/sdcard/device.cfg"

esp_err_t config_save(float mag_offset, float qnh) {
    FILE *f = fopen(CONFIG_PATH, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open config file for writing");
        return ESP_FAIL;
    }
    fprintf(f, "mag_offset=%.2f\n", mag_offset);
    fprintf(f, "qnh=%.2f\n", qnh);
    fclose(f);
    ESP_LOGI(TAG, "Configuration saved to SD: MagOffset=%.1f, QNH=%.1f", mag_offset, qnh);
    return ESP_OK;
}

esp_err_t config_load() {
    FILE *f = fopen(CONFIG_PATH, "r");
    if (f == NULL) {
        ESP_LOGI(TAG, "No config file found on SD card.");
        return ESP_ERR_NOT_FOUND;
    }

    char line[64];
    float mag = 0, qnh = 1013.25f;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "mag_offset=%f", &mag) == 1) {
            mag_handler_set_offset_direct(mag);
        } else if (sscanf(line, "qnh=%f", &qnh) == 1) {
            bme280_handler_set_qnh(qnh);
        }
    }
    fclose(f);
    ESP_LOGI(TAG, "Configuration loaded from SD: MagOffset=%.1f, QNH=%.1f", mag, qnh);
    return ESP_OK;
}
