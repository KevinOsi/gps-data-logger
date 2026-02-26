#include "hw_config.h"
#include "telemetry.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "HW_CONFIG";

global_telemetry_t g_telemetry;
SemaphoreHandle_t g_telemetry_mutex = NULL;

esp_err_t hw_config_init() {
    // 1. Initialize Global Telemetry Mutex
    g_telemetry_mutex = xSemaphoreCreateMutex();
    if (g_telemetry_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create telemetry mutex");
        return ESP_FAIL;
    }
    memset(&g_telemetry, 0, sizeof(global_telemetry_t));

    // 2. Initialize I2C Manager (This handles driver installation exactly once)
    if (i2c_manager_init(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ) != ESP_OK) {
        ESP_LOGE(TAG, "I2C Manager initialization failed");
        return ESP_FAIL;
    }

    // 3. Configure GPS UART
    uart_config_t uart_config = {
        .baud_rate = GPS_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    ESP_ERROR_CHECK(uart_driver_install(GPS_UART_NUM, 2048, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(GPS_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(GPS_UART_NUM, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(TAG, "Hardware initialized successfully");
    return ESP_OK;
}
