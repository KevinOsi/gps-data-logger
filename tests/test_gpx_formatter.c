#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mock_esp_idf.h"
#include "telemetry.h"
#include "gpx_formatter.h"

void test_gpx_point_formatting() {
    printf("Testing GPX Point Formatting...\n");
    
    global_telemetry_t snapshot = {0};
    snapshot.gps.lat = 510447000; // 51.0447 N
    snapshot.gps.lon = -1140719000; // 114.0719 W
    snapshot.env.altitude = 1045.5;
    snapshot.gps.year = 2026;
    snapshot.gps.month = 2;
    snapshot.gps.day = 23;
    snapshot.gps.hour = 22;
    snapshot.gps.minute = 0;
    snapshot.gps.second = 0;
    
    char buffer[512];
    size_t len = gpx_format_point(buffer, sizeof(buffer), &snapshot);
    
    assert(len > 0);
    assert(strstr(buffer, "lat=\"51.044700\"") != NULL);
    assert(strstr(buffer, "lon=\"-114.071900\"") != NULL);
    assert(strstr(buffer, "<ele>1045.5") != NULL);
    assert(strstr(buffer, "2026-02-23T22:00:00Z") != NULL);
    
    printf("GPX Point Formatting Test Passed!\n");
}

int main() {
    test_gpx_point_formatting();
    printf("All GPX tests passed!\n");
    return 0;
}
