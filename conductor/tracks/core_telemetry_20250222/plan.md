# Implementation Plan: Implement Core Telemetry & GPS Parsing

## Phase 1: Foundation & Shared Structures [checkpoint: 478eb14]
- [x] Task: Define `global_telemetry_t` and `ubx_nav_pvt_t` structures. [5bf5138]
    - [x] Create `main/telemetry.h`.
    - [x] Ensure all 92 bytes of the UBX-NAV-PVT payload are correctly mapped.
- [x] Task: Implement the UBX-PVT state machine parser. [9c50142]
    - [x] Create `main/gps_parser.h` and `main/gps_parser.c`.
    - [x] Implement checksum verification logic.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Foundation' (Protocol in workflow.md)

## Phase 2: Telemetry Task & UART Configuration [checkpoint: c10bddf]
- [x] Task: Configure ESP32 Hardware UART and I2C. [db0f444]
    - [x] Implement `main/hw_config.c` for UART2 (GPS) and I2C (Sensors).
    - [x] Set up the shared I2C mutex.
- [x] Task: Implement the high-priority Telemetry Task. [128a658]
    - [x] Create `main/telemetry_task.c`.
    - [x] Use FreeRTOS `xSemaphoreTake`/`xSemaphoreGive` for thread-safe telemetry updates.
- [x] Task: Conductor - User Manual Verification 'Phase 2: Telemetry Task' (Protocol in workflow.md)

## Phase 3: Integration & Basic Validation
- [ ] Task: Implement the main application entry point.
    - [ ] Create `main/main.c` to launch the Telemetry Task.
    - [ ] Add basic debug logging of GPS coordinates to verify parsing.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
