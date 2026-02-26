# Wireless Connectivity Documentation

This document describes the wireless interfaces (BLE and Wi-Fi) implemented in the GPS Data Logger.

## Bluetooth Low Energy (BLE)

The device advertises as **"GPS-Logger"**.

### 1. Environmental Sensing Service (UUID: `0x181A`)
This follows the standard Bluetooth SIG service for environmental data.

| Characteristic | UUID | Format | Description |
| :--- | :--- | :--- | :--- |
| **Temperature** | `0x2A6E` | `int16` | Value in 0.01°C (e.g., 2145 = 21.45°C) |
| **Pressure** | `0x2A6D` | `uint32` | Value in 0.1 Pa (e.g., 874000 = 874.0 hPa) |
| **Humidity** | `0x2A6F` | `uint16` | Value in 0.01% (e.g., 4500 = 45.00%) |

### 2. Custom GPS Service (UUID: `DECAFBAD-1111-2222-3333-444455556666`)
Broadcasting real-time navigation data.

| Characteristic | UUID Offset | Format | Description |
| :--- | :--- | :--- | :--- |
| **Latitude** | `...-0001` | `int32` | Degrees * 10^7 |
| **Longitude** | `...-0002` | `int32` | Degrees * 10^7 |
| **Altitude** | `...-0003` | `float` | Barometric Altitude (meters) |
| **Heading** | `...-0004` | `float` | Magnetic Heading (degrees) |

### 3. Control Service (UUID: `DECAFBAD-5555-6666-7777-888899990000`)
For remote interaction with the logger.

| Characteristic | UUID Offset | Flags | Description |
| :--- | :--- | :--- | :--- |
| **POI Marker** | `...-0001` | Write | Writing any value triggers a POI in the logs. |
| **QNH Calib** | `...-0002` | R/W | Read/Write sea level pressure (hPa) as `float`. |

---

## Wi-Fi Offload Mode

Hold the physical button for **2 seconds** to toggle this mode.

- **SSID:** `GPS-Logger-Offload`
- **Password:** `password123`
- **IP Address:** `192.168.4.1`
- **Web Interface:** Access via browser at `http://192.168.4.1`

In this mode, GPS logging and BLE are paused to ensure reliable file transfer. You can browse the SD card contents and download `.CSV` and `.GPX` files directly to your mobile device or computer.
