# Implementation Plan: BME280 Environmental Sensor Integration

## Phase 1: Driver & I2C Abstraction
- [x] Task: Implement I2C shared bus manager. [f0edcf2]
    - [x] Create `main/i2c_manager.h` and `main/i2c_manager.c`.
    - [x] Provide wrapper functions for thread-safe read/write.
- [x] Task: Integrate BME280 driver. [842bfea]
    - [x] Add BME280 source and header files to the `main` component.
- [ ] Task: Conductor - User Manual Verification 'Phase 1: Driver Integration' (Protocol in workflow.md)

## Phase 2: Sensor Logic & Polling
- [ ] Task: Implement BME280 initialization.
    - [ ] Configure the sensor for Normal mode, 16x oversampling for pressure.
- [ ] Task: Update Telemetry Task for sensor polling.
    - [ ] Poll BME280 data every 100ms (10Hz).
    - [ ] Update `g_telemetry.env` structure.
- [ ] Task: Implement Altitude calculation.
    - [ ] Add altitude calculation logic based on the hypsometric formula.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: Sensor Logic' (Protocol in workflow.md)

## Phase 3: Integration & Logging
- [ ] Task: Update debug logging in `main.c`.
    - [ ] Include Temp, Pressure, and Altitude in the periodic snapshot.
- [ ] Task: Final Build & Hardware Verification.
    - [ ] Run `idf.py build`.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
