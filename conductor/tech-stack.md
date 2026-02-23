# Tech Stack: ESP32 GPS & Environmental Logger

## Hardware Platforms
* **Microcontroller:** ESP-WROOM-32 (Dual-Core, 240MHz).
* **GNSS Module:** u-blox M8030 (UART).
* **Environmental Sensor:** BME280 (I2C).
* **OLED Display:** SSD1306 (I2C).
* **Storage:** MicroSD Card (SPI).

## Software Ecosystem
* **Core Framework:** **ESP-IDF (Native C)** leveraging FreeRTOS.
* **Build System:** CMake (standard for ESP-IDF).
* **Language:** C (C99/C11 standards).

## Key Libraries & Components
* **GPS Parsing:** Custom UBX-PVT parser (optimized for 10Hz throughput).
* **Sensor Driver:** Official Bosch BME280 C driver.
* **UI Rendering:** LVGL or a lightweight SSD1306 ESP-IDF component.
* **Storage:** FATFS over VSPI (Standard ESP-IDF `esp_vfs_fat` component).

## System Architecture
* **Core 0 (Protocol Core):** Dedicated to high-speed UART (GPS) and I2C interrupts.
* **Core 1 (App Core):** Manages file system writes (SD Card) and UI updates.
* **Communication:** FreeRTOS Queues and Mutexes for thread-safe telemetry sharing.
