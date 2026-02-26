#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "esp_err.h"

/**
 * @brief Save current device configuration to SD card.
 */
esp_err_t config_save(float mag_offset, float qnh);

/**
 * @brief Load device configuration from SD card.
 */
esp_err_t config_load(void);

#endif // CONFIG_MANAGER_H
