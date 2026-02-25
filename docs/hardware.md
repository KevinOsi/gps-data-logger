# Hardware Specifications

## ESP32 DevKitC (38-pin) Pinout
This project uses an ESP-WROOM-32 38-pin development board. All connections use **GPIO numbers**.

| Component | Interface | ESP32 GPIO | Physical Side | Pin Position |
| :--- | :--- | :--- | :--- | :--- |
| **u-blox M8030** | UART2 TX | **17** | Right | 11th from top |
| **u-blox M8030** | UART2 RX | **16** | Right | 12th from top |
| **I2C Bus** | SDA | **21** | Right | 6th from top |
| **I2C Bus** | SCL | **22** | Right | 3rd from top |
| **MicroSD** | SPI MOSI | **23** | Right | 2nd from top |
| **MicroSD** | SPI MISO | **19** | Right | 8th from top |
| **MicroSD** | SPI SCK | **18** | Right | 9th from top |
| **MicroSD** | SPI CS | **5** | Right | 10th from top |
| **POI Button** | Input | **0** | Onboard | BOOT Button |

## Sensors & Peripherals
*   **GNSS:** u-blox M8030 configured for 10Hz UBX-PVT.
*   **Environmental:** BME280 for temperature, pressure, and humidity (I2C: 0x76).
*   **Magnetometer:** Memsic MMC35240 for stationary heading (I2C: 0x30).
*   **Display:** SSD1306 128x64 OLED (I2C: 0x3C).
*   **Storage:** FATFS over MicroSD (SPI).

## Wiring Diagram
![Wiring Diagram](./wiring_diagram.svg)
