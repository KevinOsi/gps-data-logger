# Track Specification: SD Card Data Logger (CSV/GPX)

## Overview
Transform the device from a real-time monitor into a data collector by recording high-rate telemetry to a MicroSD card. Data will be stored in two formats: CSV for raw analysis and GPX for mapping and visualization.

## Objectives
1.  **Hardware Init:** Configure SPI (VSPI) and mount the MicroSD card using the ESP-IDF FATFS component.
2.  **Logger Task:** Create a background task on Core 1 that handles file writes asynchronously.
3.  **Robustness:** Implement "File Sync" logic every few seconds to ensure data isn't lost if power is pulled.
4.  **Time-Based Files:** Use GPS UTC time to generate unique filenames (e.g., `20260223_LOG.csv`).

## Success Criteria
*   MicroSD card mounts successfully on boot.
*   Telemetry data is recorded to the SD card at 10Hz.
*   CSV files can be opened in Excel/Sheets with correct columns.
*   GPX files can be imported into Google Earth or Strava.
