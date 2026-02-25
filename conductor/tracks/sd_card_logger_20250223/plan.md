# Implementation Plan: SD Card Data Logger [checkpoint: 6b8f4f5]

## Phase 1: Hardware & Filesystem
- [x] Task: Initialize SPI and Mount FATFS. [b3c2495]
    - [x] Update `main/hw_config.c` to init SPI for SD card.
    - [x] Implement `sd_card_handler.c` for mounting and unmounting.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Mounting' (Protocol in workflow.md)

## Phase 2: Async Logger Task
- [x] Task: Implement Logger Task & Buffer. [5b15e33]
    - [x] Create `logger_task.c`.
    - [x] Implement a FreeRTOS queue to pass `global_telemetry_t` snapshots.
- [x] Task: Implement CSV Formatting. [5b15e33]
    - [x] Header writing and row serialization.
- [x] Task: Conductor - User Manual Verification 'Phase 2: CSV Recording' (Protocol in workflow.md)

## Phase 3: GPX & File Management
- [x] Task: Implement GPX Serialization. [9f87cfe]
    - [x] Write GPX XML structure with `<extensions>` for BME280/Mag data.
- [x] Task: Implement Dynamic Naming. [9f87cfe]
    - [x] Generate filenames from GPS timestamp.
- [~] Task: Conductor - User Manual Verification 'Phase 3: GPX & Final Polish' (Protocol in workflow.md)
