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

/**
 * @brief Set the sea level pressure (QNH) for altitude calculation.
 * 
 * @param qnh_hpa The sea level pressure in hPa.
 */
void bme280_handler_set_qnh(float qnh_hpa);

/**
 * @brief Get the current sea level pressure (QNH).
 * 
 * @return float The current sea level pressure in hPa.
 */
float bme280_handler_get_qnh(void);

#endif // BME280_HANDLER_H
