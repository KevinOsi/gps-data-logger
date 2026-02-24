# Initial Concept
Reivew the Project specifications document for details, it is a portable GPS logger running on an ESP32

# Product Guide: ESP32 GPS & Environmental Logger

## Vision
A high-precision, portable data logging station designed for hobbyists and outdoor enthusiasts who require professional-grade telemetry. The system captures high-rate GPS and environmental data, providing real-time feedback and reliable storage for post-trip analysis.

## Target Audience
* **General Hobbyists:** Individuals interested in logging their travels, hikes, or vehicle movements with more precision than a standard smartphone.
* **Outdoor Explorers:** Users requiring dependable altitude and position data in remote environments.

## Core Goals
1. **Precision Telemetry:** Achieve 10Hz data logging using UBX-PVT protocols for high-fidelity path tracking.
2. **Environmental Fusion:** Integrate barometric pressure with GPS data for sub-meter relative altitude precision.
3. **Real-time Feedback:** Provide a responsive UI (OLED) showing critical coordinates, speed, and environmental conditions.
4. **Data Reliability:** Ensure robust logging to MicroSD with frequent syncs to prevent data loss.

## Functional Requirements (MVP)
* **High-Rate GNSS:** Parse u-blox UBX packets at 10Hz.
* **Hybrid Altitude:** Smoothed elevation profile using BME280 barometric delta.
* **Stationary Heading:** High-precision magnetometer (MMC35240) for orientation tracking without movement.
* **Storage:** Concurrent logging to .CSV (raw) and .GPX (mapping) formats.
* **POI System:** Physical button interrupt to mark Points of Interest in the logs.
* **I2C Shared Bus:** Sequential initialization and mutex-protected concurrent access for OLED, BME280, and Compass.

## User Experience (UX)
* **Rugged Simplicity:** Focused, single-button interaction for marking POIs.
*   **At-a-glance Status:** Clear OLED pages for mapping data, weather, and system health.
*   **Industrial Feel:** Leveraging ESP-IDF/FreeRTOS for a deterministic, robust firmware experience.

## Roadmap & Stretch Goals
*   **Wireless Field Commander (BLE):** Mobile companion interface for real-time map visualization and remote POI triggering using Bluetooth Low Energy.
*   **Data Offload Station (Wi-Fi):** On-demand SoftAP web server for downloading GPX/CSV logs wirelessly without removing the SD card.
*   **Dynamic Calibration:** Allow setting altitude reference (QNH) via Bluetooth.

