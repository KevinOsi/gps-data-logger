# Track Specification: BME280 Data Investigation & Calibration

## Overview
This track is dedicated to diagnosing and fixing the "out of wack" BME280 data reported by the user (e.g., 9000m altitude). We will investigate the I2C communication, the compensation logic, and the unit conversions to ensure accurate environmental telemetry.

## Objectives
1.  **Hardware Verification:** Verify the sensor is correctly identified (Chip ID 0x60) and communicating on the shared bus.
2.  **Logic Debugging:** Add telemetry to capture raw vs. compensated values.
3.  **API Audit:** Ensure the Bosch v3.5.1 driver is being used correctly with respect to its floating-point vs. integer configurations.
4.  **Formula Calibration:** Verify the hypsometric formula against local sea-level pressure.

## Success Criteria
*   BME280 Chip ID correctly logged as 0x60.
*   Temperature and Pressure readings match expected ambient conditions.
*   Altitude is realistic for the user's current location.
*   Project builds and flashes successfully.
