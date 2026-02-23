#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "mock_esp_idf.h"
#include "../main/telemetry.h"
#include "../main/gps_parser.h"
#include "../main/telemetry_task.h"

// Global mock state
static uint8_t mock_gps_data[128];
static int mock_gps_data_len = 0;

// Mock implementations
SemaphoreHandle_t xSemaphoreCreateMutex() { return (void*)1; }
bool xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime) { return pdTRUE; }
bool xSemaphoreGive(SemaphoreHandle_t xSemaphore) { return pdTRUE; }
uint32_t xPortGetCoreID() { return 0; }
TickType_t xTaskGetTickCount() { return 1000; }
void vTaskDelay(TickType_t xTicksToDelay) { /* do nothing */ }

int uart_read_bytes(uart_port_t uart_num, void* buf, uint32_t len, TickType_t ticks_to_wait) {
    if (mock_gps_data_len > 0) {
        memcpy(buf, mock_gps_data, mock_gps_data_len);
        int read_len = mock_gps_data_len;
        mock_gps_data_len = 0; // consumed
        return read_len;
    }
    return 0;
}

// Global variables defined in hw_config.c/telemetry.h
SemaphoreHandle_t g_telemetry_mutex;
global_telemetry_t g_telemetry;

int main() {
    printf("Testing Telemetry Task logic...\n");
    
    // Setup mock environment
    g_telemetry_mutex = xSemaphoreCreateMutex();
    memset(&g_telemetry, 0, sizeof(g_telemetry));
    
    // Prepare a valid UBX-NAV-PVT packet in mock_gps_data
    mock_gps_data[0] = 0xB5; mock_gps_data[1] = 0x62; // Sync
    mock_gps_data[2] = 0x01; mock_gps_data[3] = 0x07; // Class/ID
    mock_gps_data[4] = 92;   mock_gps_data[5] = 0;    // Length
    memset(&mock_gps_data[6], 0, 92);                 // Payload
    
    // Set some coordinates (lat is at offset 28 in payload)
    int32_t lat = 515000000; // 51.5 deg N
    memcpy(&mock_gps_data[6 + 28], &lat, 4);
    
    // Calculate checksum
    uint8_t ck_a = 0, ck_b = 0;
    for (int i = 2; i < 98; i++) {
        ck_a += mock_gps_data[i];
        ck_b += ck_a;
    }
    mock_gps_data[98] = ck_a;
    mock_gps_data[99] = ck_b;
    mock_gps_data_len = 100;

    // Test that we can parse the mock data manually using the same logic as the task
    gps_parser_init();
    bool parsed = false;
    for (int i = 0; i < mock_gps_data_len; i++) {
        if (gps_parse_byte(mock_gps_data[i], &g_telemetry.gps)) {
            parsed = true;
        }
    }
    
    assert(parsed == true);
    assert(g_telemetry.gps.lat == 515000000);
    
    printf("Telemetry Task logic test passed!\n");
    return 0;
}
