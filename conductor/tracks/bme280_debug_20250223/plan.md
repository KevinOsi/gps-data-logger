# Implementation Plan: BME280 Data Investigation & Calibration

## Phase 1: Diagnostic Logging & Verification
- [ ] Task: Implement advanced diagnostic logging in `bme280_handler.c`.
    - [ ] Log Chip ID and I2C address during init.
    - [ ] Log raw pressure/temp before and after compensation.
- [ ] Task: Conductor - User Manual Verification 'Phase 1: Diagnostics' (Protocol in workflow.md)

## Phase 2: Logic Fix & Calibration
- [ ] Task: Fix data processing errors.
    - [ ] Ensure correct unit conversion (Pa -> hPa).
    - [ ] Audit `BME280_DOUBLE_ENABLE` usage in the project.
- [ ] Task: Calibrate altitude formula.
    - [ ] Adjust reference sea-level pressure if necessary.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: Calibration' (Protocol in workflow.md)

## Phase 3: Final Integration & Build
- [ ] Task: Build and Flash for validation.
    - [ ] Run `idf.py build flash monitor`.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: Final Validation' (Protocol in workflow.md)
