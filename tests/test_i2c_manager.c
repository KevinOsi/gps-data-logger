#include <stdio.h>
#include <assert.h>
#include "mock_esp_idf.h"
#include "../main/i2c_manager.h"

// Mock implementations
SemaphoreHandle_t xSemaphoreCreateMutex() { return (void*)1; }
bool xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime) { return pdTRUE; }
bool xSemaphoreGive(SemaphoreHandle_t xSemaphore) { return pdTRUE; }

esp_err_t i2c_master_write_read_device(i2c_port_t i2c_num, uint8_t device_address,
                                       const uint8_t* write_buffer, size_t write_size,
                                       uint8_t* read_buffer, size_t read_size,
                                       TickType_t ticks_to_wait) {
    return ESP_OK;
}

esp_err_t i2c_master_write_to_device(i2c_port_t i2c_num, uint8_t device_address,
                                     const uint8_t* write_buffer, size_t write_size,
                                     TickType_t ticks_to_wait) {
    return ESP_OK;
}

int main() {
    printf("Testing I2C Manager...\n");
    
    // Test initialization
    assert(i2c_manager_init() == ESP_OK);
    
    // Test thread-safe read/write (mocks will always return OK for now)
    uint8_t data = 0;
    assert(i2c_manager_read(0x76, 0xD0, &data, 1) == ESP_OK);
    
    printf("I2C Manager tests passed!\n");
    return 0;
}
