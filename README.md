# HELIOSTAT Controller Firmware

This repository contains firmware, hardware documentation, and validation collateral for a low-cost, student-buildable heliostat based on the Arduino GIGA R1 WiFi. The project adheres to the architecture and operational requirements captured in the system design brief.

## Repository layout

- `platformio.ini` – PlatformIO environment for building the firmware.
- `src/` – Firmware sources organized by module (solar calculations, kinematics, motion control, UI, persistence, safety, etc.).
- `docs/` – Wiring documentation, mechanical fabrication notes, bill of materials, assembly guide, and test collateral.
- `tests/` – Scripts used during bench validation and field testing.

Refer to `docs/README.md` for hardware assembly and calibration guidance.
