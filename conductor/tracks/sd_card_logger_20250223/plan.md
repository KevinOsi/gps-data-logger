# Implementation Plan: SD Card Data Logger [checkpoint: b792361]

## Phase 1: Hardware & Filesystem
- [x] Task: Initialize SPI and Mount FATFS. [b3c2495]
    - [x] Update `main/hw_config.c` to init SPI for SD card.
    - [x] Implement `sd_card_handler.c` for mounting and unmounting.
- [x] Task: Conductor - User Manual Verification 'Phase 1: Mounting' (Protocol in workflow.md)

## Phase 2: Async Logger Task
- [ ] Task: Implement Logger Task & Buffer.
    - [ ] Create `logger_task.c`.
    - [ ] Implement a FreeRTOS queue to pass `global_telemetry_t` snapshots.
- [ ] Task: Implement CSV Formatting.
    - [ ] Header writing and row serialization.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: CSV Recording' (Protocol in workflow.md)

## Phase 3: GPX & File Management
- [ ] Task: Implement GPX Serialization.
    - [ ] Write GPX XML structure with `<extensions>` for BME280/Mag data.
- [ ] Task: Implement Dynamic Naming.
    - [ ] Generate filenames from GPS timestamp.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: GPX & Final Polish' (Protocol in workflow.md)
