#include "i2c_manager.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <string.h>

static const char *TAG = "I2C_MANAGER";
static SemaphoreHandle_t i2c_mutex = NULL;

esp_err_t i2c_manager_init(int sda_pin, int scl_pin, uint32_t clk_speed) {
    if (i2c_mutex == NULL) {
        i2c_mutex = xSemaphoreCreateMutex();
    }

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_pin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = clk_speed,
    };

    esp_err_t ret = i2c_param_config(I2C_NUM_0, &conf);
    if (ret != ESP_OK) return ret;

    // Increase timeout to handle clock stretching from sensors
    i2c_set_timeout(I2C_NUM_0, 400000); // ~10ms at 80MHz

    return i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

esp_err_t i2c_manager_write(uint8_t addr, uint8_t reg, const uint8_t *data, size_t len) {
    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return ESP_ERR_TIMEOUT;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    if (len > 0 && data != NULL) {
        i2c_master_write(cmd, (uint8_t*)data, len, true);
    }
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    xSemaphoreGive(i2c_mutex);
    return ret;
}

esp_err_t i2c_manager_read(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return ESP_ERR_TIMEOUT;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    xSemaphoreGive(i2c_mutex);
    return ret;
}
