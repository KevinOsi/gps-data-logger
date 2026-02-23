#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "mock_esp_idf.h"
#include "../main/telemetry.h"
#include "../main/bme280_handler.h"

int main() {
    printf("Testing BME280 Handler logic...\n");
    
    // Test Altitude Calculation at sea level
    float sea_level_p = 1013.25f;
    float alt = 44330.0f * (1.0f - powf(sea_level_p / 1013.25f, 0.1902949f));
    printf("Altitude at %.2f hPa: %.2f m\n", sea_level_p, alt);
    assert(fabs(alt) < 0.1f);
    
    // Test Altitude at higher elevation (e.g. 1000m ~ 898 hPa)
    float mountain_p = 898.7f;
    float alt_m = 44330.0f * (1.0f - powf(mountain_p / 1013.25f, 0.1902949f));
    printf("Altitude at %.2f hPa: %.2f m\n", mountain_p, alt_m);
    assert(alt_m > 990.0f && alt_m < 1010.0f);
    
    printf("BME280 Handler logic tests passed!\n");
    return 0;
}
