#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "esp_err.h"
#include "telemetry.h"

/**
 * @brief Switch the system to a new operating mode.
 * 
 * @param mode The new mode to switch to.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t system_manager_set_mode(system_mode_t mode);

/**
 * @brief Get the current system operating mode.
 * 
 * @return system_mode_t The current mode.
 */
system_mode_t system_manager_get_mode(void);

#endif // SYSTEM_MANAGER_H
