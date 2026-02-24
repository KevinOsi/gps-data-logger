#ifndef MAG_HANDLER_H
#define MAG_HANDLER_H

#include <stdint.h>
#include "esp_err.h"
#include "telemetry.h"

/**
 * @brief Initialize the MMC5603 magnetometer
 * @return ESP_OK on success
 */
esp_err_t mag_handler_init(void);

/**
 * @brief Read data from the magnetometer
 * @param data Pointer to store the data
 * @return ESP_OK on success
 */
esp_err_t mag_handler_read(mag_data_t *data);

#endif // MAG_HANDLER_H
