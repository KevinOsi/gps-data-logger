# Implementation Plan: Wireless Connectivity (BT/Wi-Fi)

## Phase 1: BLE Telemetry Service
- [x] Task: Integrate NimBLE stack into the project configuration (`sdkconfig`). 176f884
- [x] Task: Create `ble_manager.c` to handle BLE initialization and advertising. 176f884
- [x] Task: Define GATT services and characteristics for Telemetry (Read/Notify) and Control (Write for POI/QNH). 176f884
- [x] Task: Update `telemetry_task.c` to push telemetry snapshots to the BLE manager. 176f884

## Phase 2: Remote POI & Calibration
- [x] Task: Implement BLE write callbacks to trigger `poi_flag` in the logger logic. e82ccaa
- [x] Task: Implement BLE write callbacks to update QNH (altitude reference) dynamically. e82ccaa
- [x] Task: Create unit tests to verify GATT functionality (mocked BLE). e82ccaa

## Phase 3: Wi-Fi SoftAP Offload Mode
- [x] Task: Add UI state to toggle "Offload Mode" (Wi-Fi AP) via a long button press or menu. 7f45560
- [x] Task: Create `wifi_manager.c` to initialize SoftAP mode when toggled. 7f45560
- [x] Task: Implement `web_server.c` using `esp_http_server` to serve SD card files (`/sdcard/`). 7f45560
- [x] Task: Ensure proper teardown of BLE when Wi-Fi starts to free memory, and vice versa. 7f45560

## Phase 4: Final Polish
- [x] Task: Add UI screens to show BLE status (advertising/connected) and Wi-Fi status (SSID/IP Address). 7f45560
- [~] Task: Conductor - User Manual Verification (Connect via generic BLE app like nRF Connect, and test Web server).
    - Status: Currently performing long-term stability soak test.
