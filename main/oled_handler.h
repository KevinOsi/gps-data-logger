#ifndef OLED_HANDLER_H
#define OLED_HANDLER_H

#include "esp_err.h"

/**
 * @brief Initialize the SSD1306 OLED display.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t oled_handler_init();

/**
 * @brief Display a simple test message.
 * 
 * @param message The string to display.
 */
void oled_handler_test_message(const char *message);

#endif // OLED_HANDLER_H
