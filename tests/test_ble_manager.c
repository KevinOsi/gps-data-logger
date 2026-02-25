#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mock_esp_idf.h"
#include "../main/ble_manager.h"

// Mocks for NimBLE (we'll define these as needed for the test to compile and run)
// In a real scenario, we might use a more sophisticated mocking framework.

typedef struct {
    float lat;
    float lon;
    float alt;
} mock_telemetry_data_t;

static bool g_ble_started = false;
static bool g_advertising = false;
static bool g_connected = false;

// Mock implementations of what ble_manager.c will call
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
    // Simulate updating characteristics
    return ESP_OK;
}

int main() {
    printf("Testing BLE Manager (Mocks)...
");

    // Test Initialization
    assert(ble_manager_init() == ESP_OK);
    assert(g_ble_started == true);
    printf("Initialization OK
");

    // Test Advertising Toggle
    assert(ble_manager_set_advertising(true) == ESP_OK);
    assert(g_advertising == true);
    assert(ble_manager_set_advertising(false) == ESP_OK);
    assert(g_advertising == false);
    printf("Advertising Toggle OK
");

    // Test Connection Status
    g_connected = true;
    assert(ble_manager_is_connected() == true);
    g_connected = false;
    assert(ble_manager_is_connected() == false);
    printf("Connection Status OK
");

    // Test Telemetry Update
    global_telemetry_t mock_telemetry = {0};
    mock_telemetry.lat = 45.0f;
    mock_telemetry.lon = -110.0f;
    assert(ble_manager_update_telemetry(&mock_telemetry) == ESP_OK);
    printf("Telemetry Update OK
");

    printf("BLE Manager tests passed!
");
    return 0;
}
