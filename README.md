# HELIOSTAT Documentation

This repository contains supporting documentation for the heliostat project, including wiring schematics, mechanical drawings, and assembly procedures.

## Documentation Index

All generated documents reside in the [`docs/`](docs/) directory:

- [`wiring_schematic.pdf`](docs/wiring_schematic.pdf) — Printable wiring overview for power distribution, motor drivers, limit switches, GPS, LCD, joystick, and safety interlocks.
- [`wiring_schematic.svg`](docs/wiring_schematic.svg) — Editable source for the wiring schematic.
- [`mechanical_drawings.pdf`](docs/mechanical_drawings.pdf) — Summary sheet for the mirror carrier, elevation bracket, azimuth base, belt guard, and limit switch mounts.
- `mechanical_drawing_*.svg` files — Editable drawings for each mechanical component.
- [`assembly_guide.md`](docs/assembly_guide.md) — Step-by-step wiring, homing, calibration, and target-setting procedures.
- [`generate_documents.py`](docs/generate_documents.py) — Script that regenerates all diagrams and PDFs.

## Regenerating Documents

To regenerate the diagrams and PDFs, run:

```bash
python docs/generate_documents.py
```

The script produces both printable PDFs and editable SVG sources so updates can be made without external tooling.
