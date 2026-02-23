#ifndef MOCK_ESP_IDF_H
#define MOCK_ESP_IDF_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Mocking FreeRTOS
#define pdTRUE 1
#define pdFALSE 0
#define portTICK_PERIOD_MS 1
typedef void* SemaphoreHandle_t;
typedef void* TaskHandle_t;
typedef uint32_t TickType_t;

SemaphoreHandle_t xSemaphoreCreateMutex();
bool xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime);
bool xSemaphoreGive(SemaphoreHandle_t xSemaphore);
uint32_t xPortGetCoreID();
TickType_t xTaskGetTickCount();
void vTaskDelay(TickType_t xTicksToDelay);

// Mocking ESP Drivers
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1

typedef enum {
    UART_NUM_0,
    UART_NUM_1,
    UART_NUM_2,
    UART_NUM_MAX
} uart_port_t;

typedef enum {
    I2C_NUM_0,
    I2C_NUM_MAX
} i2c_port_t;

int uart_read_bytes(uart_port_t uart_num, void* buf, uint32_t len, TickType_t ticks_to_wait);

// Mocking Logging
#define ESP_LOGI(tag, format, ...) printf("LOGI [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, format, ...) printf("LOGE [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...) printf("LOGD [%s]: " format "\n", tag, ##__VA_ARGS__)

#endif // MOCK_ESP_IDF_H
