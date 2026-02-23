# Implementation Plan: Implement Core Telemetry & GPS Parsing

## Phase 1: Foundation & Shared Structures
- [x] Task: Define `global_telemetry_t` and `ubx_nav_pvt_t` structures. [5bf5138]
    - [x] Create `main/telemetry.h`.
    - [x] Ensure all 92 bytes of the UBX-NAV-PVT payload are correctly mapped.
- [ ] Task: Implement the UBX-PVT state machine parser.
    - [ ] Create `main/gps_parser.h` and `main/gps_parser.c`.
    - [ ] Implement checksum verification logic.
- [ ] Task: Conductor - User Manual Verification 'Phase 1: Foundation' (Protocol in workflow.md)

## Phase 2: Telemetry Task & UART Configuration
- [ ] Task: Configure ESP32 Hardware UART and I2C.
    - [ ] Implement `main/hw_config.c` for UART2 (GPS) and I2C (Sensors).
    - [ ] Set up the shared I2C mutex.
- [ ] Task: Implement the high-priority Telemetry Task.
    - [ ] Create `main/telemetry_task.c`.
    - [ ] Use FreeRTOS `xSemaphoreTake`/`xSemaphoreGive` for thread-safe telemetry updates.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: Telemetry Task' (Protocol in workflow.md)

## Phase 3: Integration & Basic Validation
- [ ] Task: Implement the main application entry point.
    - [ ] Create `main/main.c` to launch the Telemetry Task.
    - [ ] Add basic debug logging of GPS coordinates to verify parsing.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
