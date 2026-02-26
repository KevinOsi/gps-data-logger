#include "mag_handler.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "MAG_HANDLER";

#define MAG_ADDR            0x30
#define MAG_DATA_START      0x01
#define MAG_STATUS          0x00
#define MAG_PRODUCT_ID      0x07
#define MAG_CTRL_0          0x08

static float offset_x = 0.0f;
static float offset_y = 0.0f;
static float g_heading_offset = 0.0f;

esp_err_t mag_handler_init(void) {
    uint8_t chip_id = 0;
    if (i2c_manager_read(MAG_ADDR, MAG_PRODUCT_ID, &chip_id, 1) != ESP_OK) return ESP_FAIL;
    
    uint8_t set_cmd = 0x20; 
    i2c_manager_write(MAG_ADDR, MAG_CTRL_0, &set_cmd, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    uint8_t reset_cmd = 0x40;
    i2c_manager_write(MAG_ADDR, MAG_CTRL_0, &reset_cmd, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    return ESP_OK;
}

esp_err_t mag_handler_read(mag_data_t *data) {
    uint8_t tm = 0x01; 
    i2c_manager_write(MAG_ADDR, MAG_CTRL_0, &tm, 1);

    uint8_t status = 0;
    int retry = 20;
    while (retry--) {
        i2c_manager_read(MAG_ADDR, MAG_STATUS, &status, 1);
        if (status & 0x01) break;
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    uint8_t raw[6]; 
    if (i2c_manager_read(MAG_ADDR, MAG_DATA_START, raw, 6) != ESP_OK) return ESP_FAIL;

    uint16_t x_u = ((uint16_t)raw[1] << 8) | raw[0];
    uint16_t y_u = ((uint16_t)raw[3] << 8) | raw[2];

    data->x = (float)((int32_t)x_u - 8192);
    data->y = (float)((int32_t)y_u - 8192);

    float heading = atan2f(data->y - offset_y, data->x - offset_x) * 180.0f / M_PI;
    heading += g_heading_offset;
    
    while (heading < 0) heading += 360.0f;
    while (heading >= 360.0f) heading -= 360.0f;
    data->heading = heading;
    
    return ESP_OK;
}

void mag_handler_set_offset(float dummy) {
    mag_data_t current;
    float old_offset = g_heading_offset;
    g_heading_offset = 0;
    if (mag_handler_read(&current) == ESP_OK) {
        g_heading_offset = -current.heading;
        ESP_LOGI(TAG, "Zeroed: %.1f", g_heading_offset);
    } else {
        g_heading_offset = old_offset;
    }
}

void mag_handler_set_offset_direct(float offset) {
    g_heading_offset = offset;
}

float mag_handler_get_offset(void) {
    return g_heading_offset;
}

float mag_handler_calculate_declination(double lat, double lon) {
    // Basic approximation for Western North America (~Calgary area)
    // In a real pro device, we'd use a WMM table lookup.
    // Calgary: 51N, 114W is ~13.5 degrees East (positive)
    if (lat > 48 && lat < 55 && lon < -110 && lon > -120) {
        return 13.5f; 
    }
    return 0.0f; // Default
}
