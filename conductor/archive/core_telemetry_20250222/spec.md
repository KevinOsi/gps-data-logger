# Track Specification: Implement Core Telemetry & GPS Parsing

## Overview
This track focuses on establishing the foundational data handling for the GPS Logger. It includes the shared telemetry data structure, the core GPS parsing logic for UBX-PVT packets, and the high-priority task for managing real-time data flow.

## Objectives
1.  **Define Global Telemetry:** Create a thread-safe structure (`global_telemetry_t`) to hold GPS, BME280, and magnetometer data.
2.  **Implement UBX Parser:** Develop a state-machine based parser for the 92-byte `UBX-NAV-PVT` payload to achieve 10Hz throughput.
3.  **Telemetry Task (Core 0):** Implement a high-priority FreeRTOS task to poll UART for GPS data and update the global telemetry structure using mutexes.
4.  **Hardware Initialization:** Configure ESP32 UART2 and I2C buses according to the project specifications.

## Success Criteria
*   Successfully parse `UBX-NAV-PVT` packets at 10Hz with valid checksums.
*   Update `global_telemetry_t` without data races or performance bottlenecks.
*   Basic console logging of coordinates and altitude.
