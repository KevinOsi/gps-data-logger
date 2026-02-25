# Software Architecture

## Core Framework
*   **Framework:** ESP-IDF v5.3 (Native C).
*   **RTOS:** FreeRTOS (Pre-emptive scheduling).

## Task Distribution
The system is divided across the two ESP32 cores to ensure real-time performance.

### Core 0 (Protocol Core)
*   **Telemetry Task:** 
    *   Priority: 10 (High).
    *   Function: Polls UART2 for GPS data at 10Hz. Polls I2C for BME280 and MMC35240 (Magnetometer) data at 10Hz. Polls SD card storage info every 30 seconds.
    *   Sync: Updates `global_telemetry_t` using a mutex.

### Core 1 (Application Core)
*   **Button Handler Task:**
    *   Priority: 10 (High).
    *   Function: Processes GPIO interrupts from the POI button. Implements debouncing and multi-press detection (Single/Double press).
*   **UI Task:**
    *   Priority: 5 (Medium).
    *   Function: Refreshes the SSD1306 OLED at 5Hz. Manages multi-page display (Navigation, Environmental, System Status) and user page cycling.
*   **Logger Task:**
    *   Priority: 2 (Low).
    *   Function: Synchronizes telemetry snapshots to raw CSV and GPX files on the MicroSD card. Handles POI marker insertion.

## Thread Safety
A global semaphore `g_telemetry_mutex` protects the `global_telemetry_t` structure, ensuring that concurrent reads from the UI/Logger tasks do not conflict with writes from the Telemetry task. Inter-task event signaling (e.g., button presses to UI) is handled via FreeRTOS Queues.
