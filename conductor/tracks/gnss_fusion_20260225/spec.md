# Specification: GNSS Optimization & Altitude Fusion

## Goal
Optimize the GNSS performance for outdoor hiking and implement a hybrid altitude fusion system that automatically calibrates the barometer using GPS data.

## Requirements
- **GNSS Optimization:**
    - Send UBX protocol commands to enable Galileo and GLONASS constellations.
    - Set the Dynamic Platform Model to "Pedestrian" for improved low-speed filtering.
- **Smart Logging:**
    - Gated Logging: Only start recording telemetry to SD once a 3D fix is acquired.
    - Automatic Filenames: Use the GPS UTC date/time for log file names.
- **Altitude Fusion:**
    - Implement a calibration state: Once a stable 3D fix is found, use the GPS `hMSL` to set the BME280's reference sea level pressure (QNH).
    - Provide a "Fused Altitude" value that uses GPS for absolute height and the Barometer for high-resolution relative changes.

## Technical Constraints
- UBX configuration requires writing to the GPS UART. Need to implement a simple UBX command generator.
- Altitude calibration must wait for a "stable" fix (e.g., vertical accuracy < 10m) to avoid calibrating to bad data.
