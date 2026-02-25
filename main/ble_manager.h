#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "telemetry.h"

/**
 * @brief Initialize the BLE stack (NimBLE) and set up services.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t ble_manager_init(void);

/**
 * @brief Update the BLE telemetry characteristics with a new snapshot.
 * 
 * @param snapshot Pointer to the latest telemetry data.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t ble_manager_update_telemetry(const global_telemetry_t *snapshot);

/**
 * @brief Start/Stop BLE advertising.
 * 
 * @param enable True to start, false to stop.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t ble_manager_set_advertising(bool enable);

/**
 * @brief Check if a BLE client is currently connected.
 * 
 * @return true if connected, false otherwise.
 */
bool ble_manager_is_connected(void);

#endif // BLE_MANAGER_H
