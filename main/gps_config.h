#ifndef GPS_CONFIG_H
#define GPS_CONFIG_H

#include "esp_err.h"

/**
 * @brief Configure the u-blox GPS module for optimal performance.
 * 
 * Sets constellations to GPS+Galileo+GLONASS and dynamic model to Pedestrian.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t gps_config_apply(void);

#endif // GPS_CONFIG_H
