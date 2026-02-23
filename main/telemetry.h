#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#else
// Mocking FreeRTOS types for local unit testing
typedef void* SemaphoreHandle_t;
#endif

// UBX-NAV-PVT Payload (92 bytes)
typedef struct __attribute__((packed)) {
    uint32_t iTOW;          // GPS time of week (ms)
    uint16_t year;          // Year (UTC)
    uint8_t  month;         // Month (UTC)
    uint8_t  day;           // Day (UTC)
    uint8_t  hour;          // Hour (UTC)
    uint8_t  minute;        // Minute (UTC)
    uint8_t  second;        // Second (UTC)
    uint8_t  valid;          // Validity flags
    uint32_t tAcc;          // Time accuracy estimate (ns)
    int32_t  nano;          // Fraction of second (ns)
    uint8_t  fixType;       // GNSSfix Type
    uint8_t  flags;         // Fix status flags
    uint8_t  flags2;        // Additional flags
    uint8_t  numSV;         // Number of satellites
    int32_t  lon;           // Longitude (1e-7 deg)
    int32_t  lat;           // Latitude (1e-7 deg)
    int32_t  height;        // Height above ellipsoid (mm)
    int32_t  hMSL;          // Height above mean sea level (mm)
    uint32_t hAcc;          // Horizontal accuracy estimate (mm)
    uint32_t vAcc;          // Vertical accuracy estimate (mm)
    int32_t  velN;          // NED north velocity (mm/s)
    int32_t  velE;          // NED east velocity (mm/s)
    int32_t  velD;          // NED down velocity (mm/s)
    int32_t  gSpeed;        // Ground Speed (2-D) (mm/s)
    int32_t  headMot;       // Heading of motion (1e-5 deg)
    uint32_t sAcc;          // Speed accuracy estimate (mm/s)
    uint32_t headAcc;       // Heading accuracy estimate (1e-5 deg)
    uint16_t pDOP;          // Position DOP (0.01)
    uint16_t flags3;        // Additional flags
    uint32_t reserved1;     // Reserved
    int32_t  headVeh;       // Heading of vehicle (1e-5 deg)
    int16_t  magDec;        // Magnetic declination (1e-2 deg)
    uint16_t magAcc;        // Magnetic declination accuracy (1e-2 deg)
} ubx_nav_pvt_t;

typedef struct {
    float temperature;      // degC
    float pressure;         // hPa
    float humidity;         // %
    float altitude;         // m (Calculated from pressure)
} bme280_data_t;

typedef struct {
    float heading;          // Degrees from North
    int16_t x, y, z;        // Raw values
} mag_data_t;

typedef struct {
    ubx_nav_pvt_t gps;
    bme280_data_t env;
    mag_data_t mag;
    bool poi_pressed;
    uint32_t last_update_ms;
} global_telemetry_t;

extern global_telemetry_t g_telemetry;
extern SemaphoreHandle_t g_telemetry_mutex;

#endif // TELEMETRY_H
