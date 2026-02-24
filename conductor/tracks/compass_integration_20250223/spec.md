# Track Specification: Compass (Magnetometer) Integration

## Overview
This track focuses on integrating the SJTQ812DICI magnetometer via the shared I2C bus. This will provide the system with a "stationary heading" (compass), allowing the user to know their orientation even when not moving (unlike GPS heading which requires velocity).

## Objectives
1.  **I2C Integration:** Connect the magnetometer to the shared I2C bus and verify communication.
2.  **Sensor Driver:** Implement or port a driver for the SJTQ812DICI.
3.  **Heading Logic:** Convert raw X, Y, Z magnetic field data into a 0-360 degree heading.
4.  **Telemetry Sync:** Update `g_telemetry.mag` with real-time heading data at 10Hz.

## Success Criteria
*   Magnetometer detected on the I2C bus.
*   Heading values change correctly when the device is rotated.
*   Compass heading displayed on the OLED UI.
*   Project builds and flashes successfully.
