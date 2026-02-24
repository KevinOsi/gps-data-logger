# ESP32 Professional GNSS & Environmental Logger

A high-precision, portable data station designed for high-rate telemetry and environmental mapping. This system leverages **ESP-IDF** and **FreeRTOS** to capture 10Hz GNSS data fused with barometric precision.

## 🚀 Features
*   **High-Rate GNSS:** 10Hz updates using u-blox UBX-PVT protocol.
*   **Hybrid Altitude Fusion:** Combines GPS hMSL with BME280 barometric pressure for sub-meter relative elevation precision.
*   **Stationary Heading:** High-precision magnetometer (MMC35240) for orientation tracking even when not in motion.
*   **Real-time UI:** Smooth 5Hz OLED display refresh showing live coordinates, altitude, heading, and fix status.
*   **Dual-Core Architecture:** 
    *   **Core 0:** Protocol management (UART GPS, I2C Sensors).
    *   **Core 1:** App logic, UI rendering, and future SD logging.
*   **Industrial Reliability:** Thread-safe I2C shared bus management and robust state-machine parsing.

## 🛠 Hardware List
*   **MCU:** ESP-WROOM-32 (38-pin DevKitC).
*   **GPS:** u-blox M8030 (UART).
*   **Env Sensor:** BME280 (I2C).
*   **Magnetometer:** MMC35240 / MMC3xxx series (I2C).
*   **Display:** SSD1306 128x64 OLED (I2C).
*   **Storage:** MicroSD Card (SPI) - *Implementation in progress*.
*   **Input:** POI Button (GPIO 4) - *Implementation in progress*.

## 📖 Documentation
See the [docs/](./docs/) folder for detailed technical sheets:
*   [Hardware & Wiring](./docs/hardware.md)
*   [Software Architecture](./docs/software.md)

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
