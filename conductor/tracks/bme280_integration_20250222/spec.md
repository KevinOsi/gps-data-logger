# Track Specification: BME280 Environmental Sensor Integration

## Overview
This track focuses on integrating the BME280 sensor into the telemetry system. It involves establishing reliable I2C communication on the shared bus, polling environmental data (Temp, Press, Hum), and implementing the pressure-to-altitude conversion required for high-precision trail mapping.

## Objectives
1.  **I2C Management:** Refine the I2C management to support multiple devices (BME280, future Compass/OLED) using the established mutex.
2.  **Sensor Driver:** Integrate the BME280 driver and configure it for standard indoor/outdoor monitoring.
3.  **Telemetry Integration:** Update `global_telemetry_t` with real-time environmental data via the Telemetry Task.
4.  **Altitude Fusion:** Implement the barometric altitude formula to provide sub-meter relative elevation changes.

## Success Criteria
*   Successful I2C communication with BME280 (verified by ID check).
*   Environmental data updated in `global_telemetry_t`.
*   Calculated altitude fluctuates correctly with pressure changes.
*   Project builds successfully for ESP32.
