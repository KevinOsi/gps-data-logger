#include "mag_handler.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "MAG_HANDLER";

/**
 * MMC35240 / MMC34160 Register Map
 * (Common in SG906 Drones)
 */
#define MAG_ADDR            0x30
#define MAG_DATA_START      0x01 // Xout LSB, Xout MSB, Yout LSB...
#define MAG_STATUS          0x00
#define MAG_PRODUCT_ID      0x07 // We found it here in the dump
#define MAG_CTRL_0          0x08 // Common Control Reg
#define MAG_CTRL_1          0x09 

// --- CALIBRATION ---
static float offset_x = 0.0f;
static float offset_y = 0.0f;

esp_err_t mag_handler_init(void) {
    uint8_t chip_id = 0;
    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (i2c_manager_read(MAG_ADDR, MAG_PRODUCT_ID, &chip_id, 1) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read ID from 0x07");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "MMC35240 detected (ID: 0x%02x)", chip_id);

    // 1. SET/RESET Pulse to clear magnetization
    // MMC35240: Reg 0x08, bit 5=SET, bit 6=RESET
    uint8_t set_cmd = 0x20; 
    i2c_manager_write(MAG_ADDR, MAG_CTRL_0, &set_cmd, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    uint8_t reset_cmd = 0x40;
    i2c_manager_write(MAG_ADDR, MAG_CTRL_0, &reset_cmd, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Magnetometer initialized (MMC35240 Map)");
    return ESP_OK;
}

esp_err_t mag_handler_read(mag_data_t *data) {
    // 1. Trigger Measurement (TM bit 0 of CTRL0)
    uint8_t tm = 0x01; 
    i2c_manager_write(MAG_ADDR, MAG_CTRL_0, &tm, 1);

    // 2. Wait for completion (bit 0 of Status)
    uint8_t status = 0;
    int retry = 20;
    while (retry--) {
        i2c_manager_read(MAG_ADDR, MAG_STATUS, &status, 1);
        if (status & 0x01) break;
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    if (!(status & 0x01)) return ESP_ERR_TIMEOUT;

    // 3. Read 6 bytes of data
    uint8_t raw[6]; 
    if (i2c_manager_read(MAG_ADDR, MAG_DATA_START, raw, 6) != ESP_OK) {
        return ESP_FAIL;
    }

    // 14-bit Memsic parsing: LSB first, then MSB (top 6 bits)
    // Formula: (MSB << 8 | LSB) 
    uint16_t x_u = ((uint16_t)raw[1] << 8) | raw[0];
    uint16_t y_u = ((uint16_t)raw[3] << 8) | raw[2];
    uint16_t z_u = ((uint16_t)raw[5] << 8) | raw[4];

    // Memsic 14-bit center is 8192 (2^13)
    data->x = (float)((int32_t)x_u - 8192);
    data->y = (float)((int32_t)y_u - 8192);
    data->z = (float)((int32_t)z_u - 8192);

    // Heading calculation
    float heading = atan2f(data->y - offset_y, data->x - offset_x) * 180.0f / M_PI;
    if (heading < 0) heading += 360.0f;
    data->heading = heading;

    ESP_LOGD(TAG, "MAG: H:%.1f | Raw X:%u Y:%u", data->heading, x_u, y_u);
    
    return ESP_OK;
}
