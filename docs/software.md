# Software Architecture

## Core Framework
*   **Framework:** ESP-IDF v5.3 (Native C).
*   **RTOS:** FreeRTOS (Pre-emptive scheduling).

## Task Distribution
The system is divided across the two ESP32 cores to ensure real-time performance.

### Core 0 (Protocol Core)
*   **Telemetry Task:** 
    *   Priority: 5 (Balanced).
    *   Function: Polls UART2 for GPS data at 10Hz. Polls I2C for BME280 and Magnetometer data at 10Hz.
    *   Fusion Logic: Automatically calculates Magnetic Declination based on GPS fix to provide **True North** heading. Calibrates QNH pressure reference once high-accuracy GPS altitude is obtained.
*   **BLE Task:**
    *   Priority: 4.
    *   Function: Handles NimBLE stack for real-time telemetry streaming to mobile apps.

### Core 1 (Application Core)
*   **Button Handler Task:**
    *   Priority: 10 (High).
    *   Function: Processes GPIO interrupts. Implements Single-Press (Page Cycle) and Long-Press (Context Actions: Wi-Fi toggle or Compass Zeroing).
*   **UI Task:**
    *   Priority: 5 (Medium).
    *   Function: Refreshes the SSD1306 OLED at 10Hz using high-speed 800kHz I2C. Implements differential line-drawing to eliminate flickering.
*   **Logger Task:**
    *   Priority: 2 (Low).
    *   Function: Synchronizes telemetry snapshots to CSV and GPX files on the MicroSD card.

## System Services
### I2C Manager
A custom thread-safe wrapper around the ESP-IDF legacy I2C driver.
*   **Mutex Protection:** Ensures sensors and display do not collide.
*   **Stability:** Implements 10ms clock-stretch timeouts and 3-attempt retry logic for the BME280.
*   **Performance:** Configured for 800kHz high-speed operation.

### Config Manager
Handles persistence of calibration data.
*   **SD Storage:** Saves `mag_offset` and `qnh` to `/sdcard/device.cfg`.
*   **Persistence:** Loads previous calibration on boot to ensure immediate accuracy without re-calibration.

## Thread Safety
A global semaphore `g_telemetry_mutex` protects the `global_telemetry_t` structure. Inter-task communication is handled via the `button_handler_get_event()` API and direct mutex-protected memory copies.
