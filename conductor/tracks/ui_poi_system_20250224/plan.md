# Implementation Plan: Interactive UI & POI System

## Phase 1: Button Handler
- [ ] Task: Implement `button_handler.c`.
    - Create a small state machine for Single/Double press detection.
    - Attach interrupt to GPIO0.
- [ ] Task: Create `button_test` unit test to verify state machine.

## Phase 2: Multi-Page UI
- [ ] Task: Update `ui_task.c` for page state.
    - Define enum for `UI_PAGE_NAV`, `UI_PAGE_ENV`, `UI_PAGE_STATUS`.
    - Implement page cycling logic.
- [ ] Task: Design and implement layout for each page.

## Phase 3: POI Logging
- [ ] Task: Update `telemetry.h` and `telemetry_task.c`.
    - Add `poi_flag` to `global_telemetry_t`.
- [ ] Task: Update `logger_task.c` and `gpx_formatter.c`.
    - Detect `poi_flag`.
    - Write "POI" markers to CSV and GPX files.

## Phase 4: Final Polish
- [ ] Task: Add "Recording" indicator and animated transitions (if possible).
- [ ] Task: Conductor - User Manual Verification (Physical button test).
