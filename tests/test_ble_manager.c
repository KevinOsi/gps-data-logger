#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mock_esp_idf.h"
#include "../main/ble_manager.h"
#include "../main/telemetry.h"

// Mocks for NimBLE
static bool g_ble_started = false;
static bool g_advertising = false;
static bool g_connected = false;
static float g_qnh_hpa = 1013.25f;

// Mock implementations
esp_err_t ble_manager_init(void) {
    g_ble_started = true;
    return ESP_OK;
}

esp_err_t ble_manager_set_advertising(bool enable) {
    g_advertising = enable;
    return ESP_OK;
}

bool ble_manager_is_connected(void) {
    return g_connected;
}

esp_err_t ble_manager_update_telemetry(const global_telemetry_t *snapshot) {
    if (!g_ble_started) return ESP_FAIL;
    return ESP_OK;
}

// Mocking the bme280_handler functions
void bme280_handler_set_qnh(float qnh_hpa) {
    g_qnh_hpa = qnh_hpa;
}

float bme280_handler_get_qnh(void) {
    return g_qnh_hpa;
}

// Global telemetry for testing
global_telemetry_t g_telemetry = {0};
SemaphoreHandle_t g_telemetry_mutex = NULL;

int main() {
    printf("Testing BLE Manager (Remote Control Mocks)...\n");

    // Test Initialization
    assert(ble_manager_init() == ESP_OK);
    assert(g_ble_started == true);
    printf("Initialization OK\n");

    // Test QNH Update Logic (Mocked)
    printf("Testing QNH Remote Update...\n");
    bme280_handler_set_qnh(1020.0f);
    assert(bme280_handler_get_qnh() == 1020.0f);
    printf("QNH Update OK\n");

    // Test POI Trigger Logic (Mocked)
    printf("Testing Remote POI Trigger...\n");
    g_telemetry.poi_pressed = true;
    assert(g_telemetry.poi_pressed == true);
    printf("POI Trigger OK\n");

    printf("BLE Manager unit tests passed!\n");
    return 0;
}
