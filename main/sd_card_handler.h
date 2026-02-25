#ifndef SD_CARD_HANDLER_H
#define SD_CARD_HANDLER_H

#include "esp_err.h"

/**
 * @brief Initialize and mount the SD card.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t sd_card_mount(void);

/**
 * @brief Unmount the SD card.
 */
void sd_card_unmount(void);

#endif // SD_CARD_HANDLER_H
