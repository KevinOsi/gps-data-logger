#include "bme280_handler.h"
#include "bme280.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "BME280_HANDLER";
static struct bme280_dev dev;

// --- CALIBRATION ---
// Standard sea level pressure is 1013.25 hPa.
// To calibrate altitude, set this to your local QNH (pressure at sea level today).
#define SEALEVEL_PRESSURE_HPA (1013.25f)

// Interface functions for Bosch Driver
static BME280_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    esp_err_t ret = i2c_manager_read(dev_addr, reg_addr, reg_data, len);
    if (ret != ESP_OK) return BME280_E_COMM_FAIL;
    return BME280_OK;
}

static BME280_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    esp_err_t ret = i2c_manager_write(dev_addr, reg_addr, reg_data, len);
    if (ret != ESP_OK) return BME280_E_COMM_FAIL;
    return BME280_OK;
}

static void user_delay_us(uint32_t period, void *intf_ptr) {
    vTaskDelay((period / 1000 + 1) / portTICK_PERIOD_MS);
}

esp_err_t bme280_handler_init() {
    int8_t rslt = BME280_OK;
    static uint8_t dev_addr = BME280_I2C_ADDR_PRIM; // 0x76
    struct bme280_settings settings;

    dev.intf_ptr = &dev_addr;
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;

    rslt = bme280_init(&dev);
    if (rslt != BME280_OK) {
        ESP_LOGW(TAG, "Failed to initialize BME280 (rslt %d). Trying 0x77...", rslt);
        dev_addr = BME280_I2C_ADDR_SEC; // Try 0x77
        rslt = bme280_init(&dev);
        if (rslt != BME280_OK) {
            ESP_LOGE(TAG, "Failed to initialize BME280 on 0x77 (rslt %d)", rslt);
            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG, "BME280 detected: Chip ID 0x%02x at addr 0x%02x", dev.chip_id, dev_addr);

    // Recommended settings for outdoor/indoor monitoring
    settings.osr_h = BME280_OVERSAMPLING_1X;
    settings.osr_p = BME280_OVERSAMPLING_16X;
    settings.osr_t = BME280_OVERSAMPLING_2X;
    settings.filter = BME280_FILTER_COEFF_16;
    settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    uint8_t settings_sel = BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP | BME280_SEL_OSR_HUM | BME280_SEL_FILTER | BME280_SEL_STANDBY;
    rslt = bme280_set_sensor_settings(settings_sel, &settings, &dev);
    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "Failed to set BME280 settings (rslt %d)", rslt);
        return ESP_FAIL;
    }

    rslt = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev);
    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "Failed to set BME280 mode (rslt %d)", rslt);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "BME280 initialized successfully");
    return ESP_OK;
}

esp_err_t bme280_handler_read(bme280_data_t *data) {
    struct bme280_data comp_data;
    int8_t rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
    if (rslt == BME280_OK) {
        data->temperature = (float)comp_data.temperature;
        data->pressure = (float)comp_data.pressure / 100.0f; // Pa to hPa
        data->humidity = (float)comp_data.humidity;
        
        // Altitude calculation: 44330 * (1.0 - pow(pressure / QNH, 0.1903))
        data->altitude = 44330.0f * (1.0f - powf(data->pressure / SEALEVEL_PRESSURE_HPA, 0.1902949f));
        
        // Advanced Diagnostic logging
        ESP_LOGD(TAG, "DIAG: P=%f hPa, T=%f C, H=%f %%, Alt=%f m", 
                 data->pressure, data->temperature, data->humidity, data->altitude);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "BME280 read failed (rslt %d)", rslt);
    }
    return ESP_FAIL;
}
