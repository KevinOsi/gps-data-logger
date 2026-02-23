# Implementation Plan: BME280 Data Investigation & Calibration

## Phase 1: Diagnostic Logging & Verification
- [x] Task: Implement advanced diagnostic logging in `bme280_handler.c`. [3038213]
    - [x] Log Chip ID and I2C address during init.
    - [x] Log raw pressure/temp before and after compensation.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Diagnostics' (Protocol in workflow.md)

## Phase 2: Logic Fix & Calibration
- [x] Task: Fix data processing errors. [2cba5e2]
    - [x] Ensure correct unit conversion (Pa -> hPa).
    - [x] Audit `BME280_DOUBLE_ENABLE` usage in the project.
- [x] Task: Calibrate altitude formula. [2cba5e2]
    - [x] Adjust reference sea-level pressure if necessary.
- [x] Task: Conductor - User Manual Verification 'Phase 2: Calibration' (Protocol in workflow.md)

## Phase 3: Final Integration & Build
- [x] Task: Build and Flash for validation. [2cba5e2]
    - [x] Run `idf.py build flash monitor`.
- [x] Task: Conductor - User Manual Verification 'Phase 3: Final Validation' (Protocol in workflow.md)
