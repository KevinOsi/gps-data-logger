# Specification: Wireless Connectivity (BT/Wi-Fi)

## Goal
Implement wireless connectivity features for the GPS Data Logger, including a Bluetooth Low Energy (BLE) service for real-time telemetry/control and a Wi-Fi SoftAP web server for downloading recorded logs.

## Requirements
- **Bluetooth Low Energy (BLE):**
    - Broadcast current telemetry data (Lat, Lon, Alt, Heading, Temp, Press, Hum).
    - Provide a characteristic to remotely trigger a "Point of Interest" (POI) mark.
    - Provide a characteristic to dynamically calibrate the altitude reference (QNH).
- **Wi-Fi SoftAP Web Server:**
    - On-demand activation (e.g., via long-pressing the physical button or a specific menu action).
    - Host a simple HTTP server.
    - Serve a directory listing of the MicroSD card contents.
    - Allow downloading of .CSV and .GPX log files.

## Technical Constraints
- The ESP32 cannot easily run BLE and Wi-Fi concurrently without significant memory and scheduling considerations. It is recommended to have distinct operating modes: default is BLE advertising, and Wi-Fi SoftAP is an explicit mode that disables BLE and starts Wi-Fi for downloading logs.
- Utilize ESP-IDF's NimBLE or Bluedroid stack for BLE. NimBLE is preferred for its lower memory footprint.
- Utilize ESP-IDF's `esp_http_server` for the web server.
