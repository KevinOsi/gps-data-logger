# Track Specification: SSD1306 OLED Display Integration

## Overview
This track focuses on adding visual output to the portable logger. We will integrate an SSD1306 OLED display (128x64) via the shared I2C bus and implement a background UI Task to render live telemetry data.

## Objectives
1.  **Driver Integration:** Add a suitable SSD1306 driver to the project.
2.  **UI Task (Core 1):** Create a low-priority task pinned to Core 1 to handle display updates at 5Hz.
3.  **Telemetry Visualization:** Design a multi-field layout to display:
    *   Sats, Fix Type, Accuracy (Header row).
    *   Latitude & Longitude.
    *   Altitude (Fused BME/GPS).
    *   Speed & Heading.
4.  **Shared I2C Stability:** Verify that OLED rendering does not interfere with 10Hz BME280 polling.

## Success Criteria
*   OLED displays "GPS Logger" on startup.
*   Coordinates and fix status update in real-time on the screen.
*   System remains stable without I2C mutex timeouts.
*   Project builds and flashes successfully.
