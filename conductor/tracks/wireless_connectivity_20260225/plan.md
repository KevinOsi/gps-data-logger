# Implementation Plan: Wireless Connectivity (BT/Wi-Fi)

## Phase 1: BLE Telemetry Service
- [ ] Task: Integrate NimBLE stack into the project configuration (`sdkconfig`).
- [ ] Task: Create `ble_manager.c` to handle BLE initialization and advertising.
- [ ] Task: Define GATT services and characteristics for Telemetry (Read/Notify) and Control (Write for POI/QNH).
- [ ] Task: Update `telemetry_task.c` to push telemetry snapshots to the BLE manager.

## Phase 2: Remote POI & Calibration
- [ ] Task: Implement BLE write callbacks to trigger `poi_flag` in the logger logic.
- [ ] Task: Implement BLE write callbacks to update QNH (altitude reference) dynamically.
- [ ] Task: Create unit tests to verify GATT functionality (mocked BLE).

## Phase 3: Wi-Fi SoftAP Offload Mode
- [ ] Task: Add UI state to toggle "Offload Mode" (Wi-Fi AP) via a long button press or menu.
- [ ] Task: Create `wifi_manager.c` to initialize SoftAP mode when toggled.
- [ ] Task: Implement `web_server.c` using `esp_http_server` to serve SD card files (`/sdcard/`).
- [ ] Task: Ensure proper teardown of BLE when Wi-Fi starts to free memory, and vice versa.

## Phase 4: Final Polish
- [ ] Task: Add UI screens to show BLE status (advertising/connected) and Wi-Fi status (SSID/IP Address).
- [ ] Task: Conductor - User Manual Verification (Connect via generic BLE app like nRF Connect, and test Web server).
