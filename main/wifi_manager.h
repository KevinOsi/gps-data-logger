#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initialize Wi-Fi in SoftAP mode.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t wifi_manager_init_softap(void);

/**
 * @brief Shut down Wi-Fi.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t wifi_manager_stop(void);

/**
 * @brief Check if Wi-Fi is currently active.
 * 
 * @return true if active, false otherwise.
 */
bool wifi_manager_is_active(void);

#endif // WIFI_MANAGER_H
