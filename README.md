# ESP32 GNSS & Environmental Logger

Making use of salvaged toy drone parts and other odds and ends. This is A high-precision, portable data station designed for high-rate telemetry and environmental mapping. This system leverages **ESP-IDF** and **FreeRTOS** to capture 10Hz GNSS data fused with barometric precision.

## 🚀 Features
*   **High-Rate GNSS:** 10Hz updates using u-blox UBX-PVT protocol.
*   **Hybrid Altitude Fusion:** Combines GPS hMSL with BME280 barometric pressure for sub-meter relative elevation precision.
*   **Stationary Heading:** High-precision magnetometer (MMC35240) for orientation tracking even when not in motion.
*   **Interactive Multi-Page UI:** Smooth 10Hz OLED display (800kHz I2C) with four specialized pages:
    *   **GPS Status:** Real-time coordinates, altitude, and fix status.
    *   **Environmental:** Temperature, Pressure, Humidity, and Fused Altitude.
    *   **Graphical Compass:** Visual needle pointing to True North with cardinal directions.
    *   **Offload Mode:** Wi-Fi control page for data retrieval.
*   **Smart Boot Persistence:** Automatically saves/loads magnetometer offsets and pressure calibration to SD card (`device.cfg`) to speed up deployment.
*   **Dynamic True North:** Automatically calculates magnetic declination based on GPS location to correct magnetic heading to true north.
*   **Industrial Stability:** Mutex-protected I2C manager with advanced clock-stretching and retry logic to prevent bus hangs and watchdog resets.
*   **POI Marking System:** Physical button interface to mark "Points of Interest" during logging.
*   **Robust Logging:** Synchronized CSV and GPX logging to MicroSD with POI markers and environmental metadata.
*   **Dual-Core Architecture:** 
    *   **Core 0:** Protocol management (UART GPS, I2C Sensors).
    *   **Core 1:** App logic, UI rendering, Button handling, and SD logging.
*   **Industrial Reliability:** Thread-safe I2C shared bus management and robust state-machine parsing.

## 🛠 Hardware List
*   **MCU:** ESP-WROOM-32 (38-pin DevKitC).
*   **GPS:** u-blox M8030 (UART).
*   **Env Sensor:** BME280 (I2C).
*   **Magnetometer:** MMC35240 / MMC3xxx series (I2C).
*   **Display:** SSD1306 128x64 OLED (I2C).
*   **Storage:** MicroSD Card (SPI).
*   **Input:** POI Button (GPIO 0 / BOOT button for testing).

## 📖 Documentation
See the [docs/](./docs/) folder for detailed technical sheets:
*   [Hardware & Wiring](./docs/hardware.md)
*   [Software Architecture](./docs/software.md)
*   [Wireless Connectivity (BLE/Wi-Fi)](./docs/wireless.md)

## ⚡️ Quick Start

### Prerequisites
*   ESP-IDF v5.3 or later.
*   ESP32 hardware connected via USB.

### Build & Flash
1.  **Set Environment:**
    ```bash
    . $HOME/esp/esp-idf/export.sh
    ```
2.  **Initialize Project:**
    ```bash
    idf.py set-target esp32
    ```
3.  **Build and Flash:**
    ```bash
    idf.py -p /dev/ttyUSB0 build flash monitor
    ```

## 📜 License
This project is open-source. See the LICENSE file for details.
