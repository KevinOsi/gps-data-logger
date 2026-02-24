# Implementation Plan: Compass (Magnetometer) Integration

## Phase 1: Driver & Initialization
- [x] Task: Research and implement SJTQ812DICI driver. [ecc629e]
    - [x] Create `main/mag_handler.h` and `main/mag_handler.c`.
    - [x] Implement I2C detection and basic register configuration.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Driver' (Protocol in workflow.md)

## Phase 2: Heading & Calibration
- [x] Task: Implement Heading calculation. [ecc629e]
    - [x] Use `atan2(y, x)` for basic planar heading.
    - [x] Implement simple min/max calibration logic.
- [x] Task: Update Telemetry Task. [ecc629e]
    - [x] Poll magnetometer at 10Hz.
    - [x] Update `g_telemetry.mag` structure.
- [x] Task: Conductor - User Manual Verification 'Phase 2: Logic' (Protocol in workflow.md)

## Phase 3: UI & Validation
- [x] Task: Update OLED UI. [ecc629e]
    - [x] Add compass heading to the display layout.
- [x] Task: Final Build & Hardware Verification. [ecc629e]
    - [x] Run `idf.py build flash monitor`.
- [x] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
