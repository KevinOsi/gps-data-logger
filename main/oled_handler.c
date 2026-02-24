#include "oled_handler.h"
#include "ssd1306.h"
#include "hw_config.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "OLED_HANDLER";
SSD1306_t dev;

esp_err_t oled_handler_init() {
    ESP_LOGI(TAG, "Initializing SSD1306 OLED...");
    
    // We already initialized I2C in hw_config_init()
    // The driver expects to be added to an existing port
    i2c_device_add(&dev, I2C_MASTER_NUM, -1, I2C_ADDRESS);
    
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xFF);
    
    ESP_LOGI(TAG, "OLED initialized successfully");
    return ESP_OK;
}

void oled_handler_test_message(const char *message) {
    ssd1306_clear_screen(&dev, false);
    ssd1306_display_text(&dev, 0, message, strlen(message), false);
}
