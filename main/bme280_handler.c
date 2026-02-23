#include "bme280_handler.h"
#include "bme280.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "BME280_HANDLER";
static struct bme280_dev dev;

// Interface functions for Bosch Driver
static BME280_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    return i2c_manager_read(dev_addr, reg_addr, reg_data, len) == ESP_OK ? BME280_OK : BME280_E_COMM_FAIL;
}

static BME280_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    return i2c_manager_write(dev_addr, reg_addr, reg_data, len) == ESP_OK ? BME280_OK : BME280_E_COMM_FAIL;
}

static void user_delay_us(uint32_t period, void *intf_ptr) {
    vTaskDelay((period / 1000 + 1) / portTICK_PERIOD_MS);
}

esp_err_t bme280_handler_init() {
    int8_t rslt = BME280_OK;
    static uint8_t dev_addr = BME280_I2C_ADDR_PRIM; // 0x76

    dev.intf_ptr = &dev_addr;
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;

    rslt = bme280_init(&dev);
    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "Failed to initialize BME280 (rslt %d)", rslt);
        return ESP_FAIL;
    }

    // Recommended settings for outdoor/indoor monitoring
    dev.settings.osr_h = BME280_OVERSAMPLING_1X;
    dev.settings.osr_p = BME280_OVERSAMPLING_16X;
    dev.settings.osr_t = BME280_OVERSAMPLING_2X;
    dev.settings.filter = BME280_FILTER_COEFF_16;
    dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL | BME280_STAND_BY_SEL;
    rslt = bme280_set_sensor_settings(settings_sel, &dev);
    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "Failed to set BME280 settings (rslt %d)", rslt);
        return ESP_FAIL;
    }

    rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);
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
        data->temperature = comp_data.temperature;
        data->pressure = comp_data.pressure / 100.0f; // Pa to hPa
        data->humidity = comp_data.humidity;
        
        // Altitude calculation: 44330 * (1.0 - pow(pressure / 1013.25, 0.1903))
        data->altitude = 44330.0f * (1.0f - powf(data->pressure / 1013.25f, 0.1902949f));
        return ESP_OK;
    }
    return ESP_FAIL;
}
