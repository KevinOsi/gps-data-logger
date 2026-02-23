#ifndef BME280_HANDLER_H
#define BME280_HANDLER_H

#include "esp_err.h"
#include "telemetry.h"

/**
 * @brief Initialize the BME280 sensor.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t bme280_handler_init();

/**
 * @brief Read data from the BME280 and update the telemetry structure.
 * 
 * @param data Pointer to the bme280_data_t structure to update.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t bme280_handler_read(bme280_data_t *data);

#endif // BME280_HANDLER_H
