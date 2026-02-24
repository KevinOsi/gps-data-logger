# Implementation Plan: Compass (Magnetometer) Integration

## Phase 1: Driver & Initialization
- [ ] Task: Research and implement SJTQ812DICI driver.
    - [ ] Create `main/mag_handler.h` and `main/mag_handler.c`.
    - [ ] Implement I2C detection and basic register configuration.
- [ ] Task: Conductor - User Manual Verification 'Phase 1: Driver' (Protocol in workflow.md)

## Phase 2: Heading & Calibration
- [ ] Task: Implement Heading calculation.
    - [ ] Use `atan2(y, x)` for basic planar heading.
    - [ ] Implement simple min/max calibration logic.
- [ ] Task: Update Telemetry Task.
    - [ ] Poll magnetometer at 10Hz.
    - [ ] Update `g_telemetry.mag` structure.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: Logic' (Protocol in workflow.md)

## Phase 3: UI & Validation
- [ ] Task: Update OLED UI.
    - [ ] Add compass heading to the display layout.
- [ ] Task: Final Build & Hardware Verification.
    - [ ] Run `idf.py build flash monitor`.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
