# **Project Specification: ESP32 Professional GNSS & Environmental Logger**

## **1\. Project Overview**

An industrial-grade portable data station using the **ESP-WROOM-32**. This system leverages **ESP-IDF** and **FreeRTOS** to capture high-rate (10Hz) telemetry. It fuses GNSS data from a u-blox M8030 with atmospheric data from a BME280 to provide high-precision trail mapping, altitude profiling, and environmental logging.

## **2\. Hardware Architecture**

### **2.1 Core Controller: ESP-WROOM-32**

* **Toolchain:** ESP-IDF (Native C/FreeRTOS).  
* **Core 0 (Protocol Core):** Manages UART (GPS) and I2C (Sensors/OLED) interrupts.  
* **Core 1 (App Core):** Manages SPI (SD Card) writes and UI rendering logic.

### **2.2 Sensors & Peripherals**

* **GNSS:** u-blox M8030 (UART2) configured for 10Hz UBX-PVT.  
* **Environmental:** BME280 (I2C) for pressure, temperature, and humidity.  
* **Compass:** External I2C Magnetometer (SJTQ812DICI) for stationary heading.  
* **Storage:** MicroSD (SPI) using FATFS for PC-readable logs.  
* **Display:** SSD1306 OLED (I2C) for real-time telemetry.  
* **Input:** Physical "POI" (Point of Interest) button on GPIO 4\.

## **3\. Hardware Pin Mapping (ESP32)**

| Component | Interface | ESP32 Pin | Notes |
| :---- | :---- | :---- | :---- |
| **u-blox M8030** | UART2 RX/TX | GPIO 16 / 17 | Dedicated Hardware Serial |
| **MicroSD Card** | SPI (VSPI) | GPIO 5(CS), 18(CLK), 19(MISO), 23(MOSI) | High-speed SPI |
| **I2C Bus** | SDA / SCL | GPIO 21 / 22 | Shared: OLED, BME280, Compass |
| **POI Button** | Digital In | GPIO 4 | Internal Pull-up, Interrupt-driven |

## **4\. Precision & Data Strategy**

### **4.1 Hybrid Altitude Fusion**

GPS altitude is often inaccurate due to satellite geometry. We will implement:

* **Baseline:** GPS hMSL (Height above Mean Sea Level) for the absolute start point.  
* **Delta:** BME280 Barometric Pressure to track relative changes with sub-meter precision (Pressure Alt).  
* **Result:** A smoothed, high-accuracy elevation profile for trail mapping.

### **4.2 File Formats**

The Logger\_Task will generate two files per session:

1. **LOG\_XXXX.CSV:** Raw data dump (all UBX fields, raw BME280 values, raw Compass heading).  
2. **TRAIL\_XXXX.GPX:** Standard XML format for immediate import into mapping software (Google Earth, Garmin, AllTrails).  
   * *Includes \<extensions\> tags for Temp/Humidity/Pressure data.*

### **4.3 GNSS Optimization**

* **Constellations:** Enable GPS \+ Galileo \+ GLONASS concurrently.  
* **Navigation Model:** Set to Pedestrian via UBX-CFG-NAV5 to improve filtering for walking speeds.

## **5\. Software Architecture (FreeRTOS Tasks)**

### **Task 1: Telemetry\_Task (High Priority \- Core 0\)**

* **GPS:** Parse UBX packets from UART2.  
* **Sensors:** Poll BME280 and Compass via I2C.  
* **Sync:** Update a thread-safe GlobalTelemetry struct (using Mutex).

### **Task 2: Logger\_Task (Low Priority \- Core 1\)**

* **Trigger:** Wait for "Data Ready" signal from Telemetry Task.  
* **Write:** Append formatted strings to SD card.  
* **Reliability:** Trigger fsync() every 5 seconds or upon POI button press to ensure data isn't lost if power is cut.

### **Task 3: UI\_Task (Low Priority \- Core 1\)**

* **Render:** Update OLED at 5Hz.  
* **Pages:** Cycle between "Mapping" (Coords/Alt), "Weather" (Temp/Hum/Baro), and "System" (Sats/Accuracy/SD Status).

## **6\. Implementation & Safety Plan**

1. **I2C Collision Management:** Since three devices share one I2C bus, use an I2C Mutex in ESP-IDF to prevent the OLED task from interrupting a BME280 read.  
2. **Startup Sequence:**  
   * Scan I2C for Compass Address (0x1E vs 0x0D).  
   * Initialize SD Card; if failed, alert on OLED.  
   * Wait for GPS fixType \>= 3 before opening the log file to ensure clean start coordinates.  
3. **Power Supply:** Use a 1000uF capacitor across the 5V/3.3V rail. SD card "write" spikes can cause brownouts on the ESP32 or noise on the GPS antenna.
