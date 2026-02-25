#ifndef SD_CARD_HANDLER_H
#define SD_CARD_HANDLER_H

#include "esp_err.h"
#include <stdint.h>

/**
 * @brief Initialize and mount the SD card.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t sd_card_mount(void);

/**
 * @brief Get SD card storage information.
 * 
 * @param out_total_mb Pointer to store total capacity in MB.
 * @param out_free_mb Pointer to store free space in MB.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t sd_card_get_info(uint32_t *out_total_mb, uint32_t *out_free_mb);

/**
 * @brief Unmount the SD card.
 */
void sd_card_unmount(void);

#endif // SD_CARD_HANDLER_H
