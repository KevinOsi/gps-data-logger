# Product Guidelines: ESP32 GPS & Environmental Logger

## Core Principles
1. **Professional Documentation:** Use technical, precise language for internal documentation and developer-facing logs.
2. **Minimalist UI:** The OLED should present only the most relevant data at any given time, avoiding clutter.
3. **Robust Error Handling:** The system must gracefully handle sensor failures or SD card disconnection, providing clear alerts to the user.
4. **Data Integrity:** Prioritize logging over UI smoothness if resources are constrained.

## Design System (OLED)
* **Typography:** Clear, high-contrast monospace fonts for readability.
* **Layout:** A grid-based approach for data fields (e.g., Satellites, Fix Type, Accuracy in a header row).
* **Feedback:** Visual confirmation (flashing icon or text) when a POI is successfully marked or data is written to the SD card.

## Development Style
* **Safety First:** Avoid blocking the main telemetry task. Use FreeRTOS queues for data passing.
* **Traceability:** Every major event (start, stop, POI, errors) should be logged with a timestamp in a separate debug log.
* **Hardware-Awareness:** Implement power-saving measures where possible without compromising the 10Hz sampling rate.
