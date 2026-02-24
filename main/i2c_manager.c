#include "i2c_manager.h"
#include "hw_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "I2C_MANAGER";
static SemaphoreHandle_t i2c_mutex = NULL;

esp_err_t i2c_manager_init() {
    if (i2c_mutex != NULL) return ESP_OK; // Already initialized
    
    i2c_mutex = xSemaphoreCreateMutex();
    if (i2c_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create I2C mutex");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t i2c_manager_read(uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t len) {
    esp_err_t ret = ESP_FAIL;
    if (xSemaphoreTake(i2c_mutex, 250 / portTICK_PERIOD_MS) == pdTRUE) {
        ret = i2c_master_write_read_device(I2C_MASTER_NUM, device_address, 
                                           &reg_address, 1, data, len, 
                                           200 / portTICK_PERIOD_MS);
        xSemaphoreGive(i2c_mutex);
    } else {
        ESP_LOGE(TAG, "I2C Mutex timeout (read)");
    }
    return ret;
}

esp_err_t i2c_manager_write(uint8_t device_address, uint8_t reg_address, const uint8_t *data, size_t len) {
    esp_err_t ret = ESP_FAIL;
    uint8_t write_buf[len + 1];
    write_buf[0] = reg_address;
    if (data && len > 0) {
        memcpy(&write_buf[1], data, len);
    }

    if (xSemaphoreTake(i2c_mutex, 250 / portTICK_PERIOD_MS) == pdTRUE) {
        ret = i2c_master_write_to_device(I2C_MASTER_NUM, device_address, 
                                         write_buf, len + 1, 
                                         200 / portTICK_PERIOD_MS);
        xSemaphoreGive(i2c_mutex);
    } else {
        ESP_LOGE(TAG, "I2C Mutex timeout (write)");
    }
    return ret;
}
