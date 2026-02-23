#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../main/gps_parser.h"
#include "../main/telemetry.h"

// Helper to calculate UBX checksum
void calculate_ubx_checksum(uint8_t *data, size_t len, uint8_t *ck_a, uint8_t *ck_b) {
    *ck_a = 0;
    *ck_b = 0;
    for (size_t i = 0; i < len; i++) {
        *ck_a += data[i];
        *ck_b += *ck_a;
    }
}

int main() {
    printf("Testing GPS parser...\n");
    
    ubx_nav_pvt_t pvt_out;
    gps_parser_init();
    
    // Construct a valid UBX-NAV-PVT packet
    uint8_t packet[100]; // 6 header + 92 payload + 2 checksum
    packet[0] = 0xB5; // Sync1
    packet[1] = 0x62; // Sync2
    packet[2] = 0x01; // Class (NAV)
    packet[3] = 0x07; // ID (PVT)
    packet[4] = 92;   // Length LSB
    packet[5] = 0;    // Length MSB
    
    // Fill payload with some data
    memset(&packet[6], 0xAA, 92);
    // Overwrite some specific fields
    uint16_t year = 2026;
    memcpy(&packet[6 + 4], &year, 2); // year is at offset 4 in payload
    
    // Calculate checksum for bytes after sync chars up to end of payload
    uint8_t ck_a, ck_b;
    calculate_ubx_checksum(&packet[2], 96, &ck_a, &ck_b);
    packet[98] = ck_a;
    packet[99] = ck_b;
    
    // Feed packet byte by byte
    bool result = false;
    for (int i = 0; i < 100; i++) {
        result = gps_parse_byte(packet[i], &pvt_out);
        if (i < 99) {
            assert(result == false);
        }
    }
    
    assert(result == true);
    assert(pvt_out.year == 2026);
    
    printf("GPS parser test passed!\n");
    return 0;
}
