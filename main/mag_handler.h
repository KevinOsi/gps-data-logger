#ifndef MAG_HANDLER_H
#define MAG_HANDLER_H

#include "esp_err.h"
#include <stdint.h>
#include "telemetry.h" // Use centralized mag_data_t

esp_err_t mag_handler_init(void);
esp_err_t mag_handler_read(mag_data_t *data);
void mag_handler_set_offset(float dummy);
void mag_handler_set_offset_direct(float offset);
float mag_handler_get_offset(void);

/**
 * @brief Calculate magnetic declination for a given lat/lon.
 */
float mag_handler_calculate_declination(double lat, double lon);

#endif // MAG_HANDLER_H
