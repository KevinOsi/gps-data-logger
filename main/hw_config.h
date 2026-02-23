#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include "driver/uart.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

// GPS (u-blox M8030) UART2
#define GPS_UART_NUM      UART_NUM_2
#define GPS_TX_PIN        17
#define GPS_RX_PIN        16
#define GPS_BAUD_RATE     115200

// I2C Shared Bus (OLED, BME280, Compass)
#define I2C_MASTER_NUM    I2C_NUM_0
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000

// SD Card (VSPI)
#define SD_CS_PIN         5
#define SD_SCK_PIN        18
#define SD_MISO_PIN       19
#define SD_MOSI_PIN       23

// User Input
#define POI_BUTTON_PIN    4

/**
 * @brief Initialize all hardware peripherals.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t hw_config_init();

#endif // HW_CONFIG_H
