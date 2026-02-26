#include "gps_config.h"
#include "hw_config.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "GPS_CONFIG";

/**
 * Send a UBX command with checksum calculation.
 */
static void send_ubx(uint8_t class, uint8_t id, uint16_t len, const uint8_t *payload) {
    uint8_t header[6] = {
        0xB5, 0x62,         // Sync
        class, id,          // ID
        len & 0xFF, (len >> 8) & 0xFF // Length
    };

    uint8_t ck_a = 0, ck_b = 0;
    
    // Checksum calculation (excluding sync bytes)
    for (int i = 2; i < 6; i++) {
        ck_a += header[i];
        ck_b += ck_a;
    }
    for (int i = 0; i < len; i++) {
        ck_a += payload[i];
        ck_b += ck_a;
    }

    uart_write_bytes(GPS_UART_NUM, (const char*)header, 6);
    if (len > 0) {
        uart_write_bytes(GPS_UART_NUM, (const char*)payload, len);
    }
    uart_write_bytes(GPS_UART_NUM, (const char*)&ck_a, 1);
    uart_write_bytes(GPS_UART_NUM, (const char*)&ck_b, 1);
    
    ESP_LOGD(TAG, "Sent UBX MSG %02X %02X", class, id);
}

esp_err_t gps_config_apply(void) {
    ESP_LOGI(TAG, "Configuring u-blox module...");

    // 1. UBX-CFG-NAV5: Set Dynamic Model to Pedestrian (3)
    uint8_t nav5_payload[36] = {0};
    nav5_payload[0] = 0x01 | 0x04; // Mask: Dynamic Model + Fix Mode
    nav5_payload[2] = 0x03;        // Dynamic Model: Pedestrian
    nav5_payload[3] = 0x02;        // Fix Mode: 3D only
    send_ubx(0x06, 0x24, 36, nav5_payload);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // 2. UBX-CFG-GNSS: Enable GPS + Galileo + GLONASS
    // M8030 supports 3 concurrent constellations.
    // GPS (0), SBAS (1), Galileo (2), BeiDou (3), IMES (4), QZSS (5), GLONASS (6)
    // Each block is 8 bytes. Header is 4 bytes.
    uint8_t gnss_payload[4 + (7 * 8)] = {0};
    gnss_payload[0] = 0; // msgVer
    gnss_payload[1] = 0; // numTrkChHw (ignored on write)
    gnss_payload[2] = 0; // numTrkChUse (ignored on write)
    gnss_payload[3] = 7; // numConfigBlocks

    // Block 0: GPS (Enable)
    gnss_payload[4] = 0;    // gnssId
    gnss_payload[8] = 1;    // Enable
    gnss_payload[10] = 0x01; // Enable bit

    // Block 1: SBAS (Enable)
    gnss_payload[12] = 1;
    gnss_payload[16] = 1;
    gnss_payload[18] = 0x01;

    // Block 2: Galileo (Enable)
    gnss_payload[20] = 2;
    gnss_payload[24] = 1;
    gnss_payload[26] = 0x01;

    // Block 3: BeiDou (Disable)
    gnss_payload[28] = 3;
    gnss_payload[32] = 0;

    // Block 4: IMES (Disable)
    gnss_payload[36] = 4;
    gnss_payload[40] = 0;

    // Block 5: QZSS (Enable)
    gnss_payload[44] = 5;
    gnss_payload[48] = 1;
    gnss_payload[50] = 0x01;

    // Block 6: GLONASS (Enable)
    gnss_payload[52] = 6;
    gnss_payload[56] = 1;
    gnss_payload[58] = 0x01;

    send_ubx(0x06, 0x3E, sizeof(gnss_payload), gnss_payload);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // 3. UBX-CFG-RATE: Set Rate to 10Hz (100ms)
    uint8_t rate_payload[6] = {
        0x64, 0x00, // 100ms
        0x01, 0x00, // 1 cycle
        0x01, 0x00  // UTC time
    };
    send_ubx(0x06, 0x08, 6, rate_payload);
    
    ESP_LOGI(TAG, "GPS Configuration Sent (Pedestrian Mode, GPS+GAL+GLO, 10Hz)");
    return ESP_OK;
}
