# Implementation Plan: BME280 Environmental Sensor Integration

## Phase 1: Driver & I2C Abstraction [checkpoint: 4b02479]
- [x] Task: Implement I2C shared bus manager. [f0edcf2]
    - [x] Create `main/i2c_manager.h` and `main/i2c_manager.c`.
    - [x] Provide wrapper functions for thread-safe read/write.
- [x] Task: Integrate BME280 driver. [842bfea]
    - [x] Add BME280 source and header files to the `main` component.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Driver Integration' (Protocol in workflow.md)

## Phase 2: Sensor Logic & Polling [checkpoint: 5a07838]
- [x] Task: Implement BME280 initialization. [95134e6]
    - [x] Configure the sensor for Normal mode, 16x oversampling for pressure.
- [x] Task: Update Telemetry Task for sensor polling. [95134e6]
    - [x] Poll BME280 data every 100ms (10Hz).
    - [x] Update `g_telemetry.env` structure.
- [x] Task: Implement Altitude calculation. [95134e6]
    - [x] Add altitude calculation logic based on the hypsometric formula.
- [x] Task: Conductor - User Manual Verification 'Phase 2: Sensor Logic' (Protocol in workflow.md)

## Phase 3: Integration & Logging [checkpoint: 759df0e]
- [x] Task: Update debug logging in `main.c`. [fb634cf]
    - [x] Include Temp, Pressure, and Altitude in the periodic snapshot.
- [x] Task: Final Build & Hardware Verification. [fb634cf]
    - [x] Run `idf.py build`.
- [x] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
