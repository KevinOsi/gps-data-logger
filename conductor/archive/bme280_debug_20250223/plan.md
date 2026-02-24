# Implementation Plan: BME280 Data Investigation & Calibration

## Phase 1: Diagnostic Logging & Verification
- [x] Task: Implement advanced diagnostic logging in `bme280_handler.c`. [3038213]
    - [x] Log Chip ID and I2C address during init.
    - [x] Log raw pressure/temp before and after compensation.
- [x] Task: Resolve I2C Mutex Starvation. [ecc629e]
    - [x] Optimize OLED character writes to reduce mutex overhead.
    - [x] Increase Mutex timeout for sensor tasks.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Diagnostics' (Protocol in workflow.md)

## Phase 2: Logic Fix & Calibration
- [x] Task: Fix data processing errors. [2cba5e2]
    - [x] Ensure correct unit conversion (Pa -> hPa).
    - [x] Audit `BME280_DOUBLE_ENABLE` usage in the project.
- [x] Task: Verify 0x30 as MMC5603/MMC35240 Magnetometer. [ecc629e]
- [x] Task: Calibrate altitude formula. [ecc629e]
    - [x] Adjust reference sea-level pressure if necessary.
- [x] Task: Conductor - User Manual Verification 'Phase 2: Calibration' (Protocol in workflow.md)

## Phase 3: Final Integration & Build
- [x] Task: Build and Flash for validation. [ecc629e]
    - [x] Run `idf.py build flash monitor`.
- [x] Task: Conductor - User Manual Verification 'Phase 3: Final Validation' (Protocol in workflow.md)
