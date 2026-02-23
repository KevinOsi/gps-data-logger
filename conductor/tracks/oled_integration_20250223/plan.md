# Implementation Plan: SSD1306 OLED Display Integration

## Phase 1: Driver & Basic Rendering
- [x] Task: Integrate SSD1306 driver. [e0a1638]
    - [x] Add a lightweight C-based SSD1306 driver to the `main` component.
- [x] Task: Implement display initialization. [e0a1638]
    - [x] Configure I2C address (0x3C) and screen parameters.
    - [x] Create a simple "Hello World" test function.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Driver & Rendering' (Protocol in workflow.md)

## Phase 2: UI Task & live Data rendering
- [ ] Task: Implement the UI Task.
    - [ ] Create `main/ui_task.c` and `main/ui_task.h`.
    - [ ] Implement a 5Hz update loop (200ms delay).
- [ ] Task: Design Live Telemetry Layout.
    *   [ ] Header: Sats, Fix, Accuracy.
    *   [ ] Body: Lat, Lon, Alt.
    *   [ ] Footer: Speed, Heading.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: UI Task' (Protocol in workflow.md)

## Phase 3: Integration & Build
- [ ] Task: Update `main.c` to launch the UI Task.
- [ ] Task: Final Build & Flash.
    - [ ] Run `idf.py build flash monitor`.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: Integration' (Protocol in workflow.md)
