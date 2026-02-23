#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/**
 * @brief Initialize the I2C shared bus manager.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t i2c_manager_init();

/**
 * @brief Thread-safe read from an I2C device.
 * 
 * @param device_address The I2C address of the device.
 * @param reg_address The register address to read from.
 * @param data Buffer to store the read data.
 * @param len Number of bytes to read.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t i2c_manager_read(uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t len);

/**
 * @brief Thread-safe write to an I2C device.
 * 
 * @param device_address The I2C address of the device.
 * @param reg_address The register address to write to.
 * @param data Buffer containing the data to write.
 * @param len Number of bytes to write.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t i2c_manager_write(uint8_t device_address, uint8_t reg_address, const uint8_t *data, size_t len);

#endif // I2C_MANAGER_H
