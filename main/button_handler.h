#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Button event types.
 */
typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_SINGLE_PRESS,
    BUTTON_EVENT_DOUBLE_PRESS,
    BUTTON_EVENT_LONG_PRESS
} button_event_t;

/**
 * @brief Initialize the button handler.
 * 
 * @param gpio_num The GPIO number for the button.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t button_handler_init(gpio_num_t gpio_num);

/**
 * @brief Get the next button event.
 * 
 * @param event Pointer to store the event.
 * @param wait_ticks Number of FreeRTOS ticks to wait for an event.
 * @return esp_err_t ESP_OK if an event was retrieved, ESP_ERR_TIMEOUT if not.
 */
esp_err_t button_handler_get_event(button_event_t *event, TickType_t wait_ticks);

#endif // BUTTON_HANDLER_H
