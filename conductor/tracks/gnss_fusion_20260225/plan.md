# Implementation Plan: GNSS Optimization & Altitude Fusion

## Phase 1: GNSS Configuration
- [x] Task: Implement `gps_config.c` to send UBX commands over UART.
- [x] Task: Add commands for Concurrent Constellation (GPS/GAL/GLO).
- [x] Task: Add command for Navigation Model (Pedestrian).
- [x] Task: Verify configuration via serial logs.

## Phase 2: Hybrid Altitude Fusion
- [x] Task: Create altitude fusion logic in `telemetry_task.c`.
- [x] Task: Implement auto-QNH calibration when GPS fix is stable.
- [x] Task: Update OLED and Web dashboards to show "Fused Alt".

## Phase 3: Smart Logging
- [x] Task: Update `logger_task.c` to wait for a 3D fix before opening files.
- [x] Task: Implement dynamic filename generation based on GPS timestamp.
- [x] Task: Add "Waiting for Fix" status to UI.

## Phase 4: Final Polish
- [ ] Task: Conductor - Outdoor Verification (Battery run).
- [ ] Task: Document the fusion algorithm in `docs/software.md`.
