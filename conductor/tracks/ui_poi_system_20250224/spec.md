# Specification: Interactive UI & POI System

## Goal
Transform the existing static OLED display into a dynamic, multi-page interface and implement a "Point of Interest" (POI) marking system using a physical button.

## Requirements
- **Button Handling:** 
    - Use GPIO0 (onboard BOOT button) for testing.
    - Implement software debouncing.
    - Support **Single Press** (Cycle UI screens) and **Double Press** (Mark POI).
- **Multi-Page UI:**
    - Page 1: **Navigation** (Lat, Lon, Alt, Heading).
    - Page 2: **Environmental** (Temp, Press, Hum).
    - Page 3: **Status** (Sats, SD space, Uptime).
- **POI System:**
    - When a POI is marked, set a flag in the telemetry snapshot.
    - The `logger_task` must detect this flag and insert a POI row into the CSV and a `<wpt>` (Waypoint) or extension into the GPX.
- **Visual Polish:**
    - Implement a small "recording" indicator (blinking dot).
    - Use differential updates to ensure a flicker-free 5Hz refresh rate.

## Hardware Mapping
- **POI Button:** GPIO0 (Active LOW).
- **Display:** SSD1306 (I2C).

## Technical Constraints
- Button interrupt must be handled on Core 1 (UI core) to avoid jitter on the Telemetry core.
- Debounce period: 50ms.
- Double-press window: 400ms.
