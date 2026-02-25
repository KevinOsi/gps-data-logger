#include "gpx_formatter.h"
#include <stdio.h>

const char* GPX_HEADER = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<gpx version=\"1.1\" creator=\"ESP32-GPS-Logger\" xmlns=\"http://www.topografix.com/GPX/1/1\">\n"
    "<trk>\n"
    "<name>Telemetry Log</name>\n"
    "<trkseg>\n";

const char* GPX_FOOTER = 
    "</trkseg>\n"
    "</trk>\n"
    "</gpx>\n";

size_t gpx_format_point(char *buffer, size_t size, const global_telemetry_t *snapshot) {
    if (buffer == NULL || size == 0 || snapshot == NULL) return 0;

    return snprintf(buffer, size,
        "<trkpt lat=\"%.6f\" lon=\"%.6f\">\n"
        "  <ele>%.1f</ele>\n"
        "  <time>%04u-%02u-%02uT%02u:%02u:%02uZ</time>\n"
        "  <extensions>\n"
        "    <temp>%.1f</temp>\n"
        "    <press>%.1f</press>\n"
        "    <hum>%.1f</hum>\n"
        "    <magH>%.1f</magH>\n"
        "  </extensions>\n"
        "</trkpt>\n",
        snapshot->gps.lat / 10000000.0,
        snapshot->gps.lon / 10000000.0,
        snapshot->env.altitude,
        snapshot->gps.year, snapshot->gps.month, snapshot->gps.day,
        snapshot->gps.hour, snapshot->gps.minute, snapshot->gps.second,
        snapshot->env.temperature,
        snapshot->env.pressure,
        snapshot->env.humidity,
        snapshot->mag.heading
    );
}

const char* gpx_get_header(void) {
    return GPX_HEADER;
}

const char* gpx_get_footer(void) {
    return GPX_FOOTER;
}
