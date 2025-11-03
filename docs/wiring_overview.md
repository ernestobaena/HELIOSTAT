# Wiring Overview & Schematic Notes

The following pages document the complete electrical architecture of the heliostat controller. Use the connection tables to wire the harness and recreate the schematic in your preferred CAD tool (KiCad/EasyEDA). A PDF export can be generated directly from these tables.

## Power Tree

| Node | Description | Notes |
| --- | --- | --- |
| BATT+ | 12 V LiFePO4 battery positive | 10–15 A fuse within 10 cm of battery. |
| BATT- | Battery return | Star ground reference. |
| MPPT IN | PV to MPPT charge controller | Sized for 50–100 W module. |
| MPPT OUT | 12 V regulated bus | Feeds drivers, DC/DC converters. |
| 12V BUS | Distribution block | Split to stepper drivers and DC/DC converters. |
| 5V BUCK | 12 V→5 V buck converter | Powers LCD, joystick, GPS if required. |
| 3V3 REG | On-board GIGA supply | Logic domain. |

## Controller Interconnects

| Subsystem | Connector | Pin | Signal | Destination |
| --- | --- | --- | --- | --- |
| TMC2209 AZ | JST-VH-6 | 1 | VMOT+ (12 V) | 12V BUS |
| | | 2 | VMOT- (GND) | BATT- |
| | | 3 | STEP | GIGA D6 |
| | | 4 | DIR | GIGA D7 |
| | | 5 | EN | GIGA D8 (active LOW) |
| | | 6 | UART | Shared bus (Serial2 TX/RX) |
| TMC2209 EL | JST-VH-6 | 1 | VMOT+ | 12V BUS |
| | | 2 | VMOT- | BATT- |
| | | 3 | STEP | GIGA D9 |
| | | 4 | DIR | GIGA D10 |
| | | 5 | EN | GIGA D11 |
| | | 6 | UART | Shared bus |
| Limit switches | M12-4 | 1 | AZ_HOME | D22 (pull-up) |
| | | 2 | AZ_MAX | D23 (pull-up) |
| | | 3 | EL_HOME | D24 (pull-up) |
| | | 4 | EL_MAX | D25 (pull-up) |
| Joystick | JST-XH-5 | 1 | +5 V | 5V BUCK |
| | | 2 | GND | BATT- |
| | | 3 | X axis | A0 |
| | | 4 | Y axis | A1 |
| | | 5 | Push | D30 |
| Buttons | JST-XH-5 | 1 | Target Acquire | D26 |
| | | 2 | Set/Start | D27 |
| | | 3 | Park | D28 |
| | | 4 | Menu | D29 |
| | | 5 | GND | BATT- |
| GPS | JST-GH-6 | 1 | +5 V | 5V BUCK |
| | | 2 | GND | BATT- |
| | | 3 | TX | Serial3 RX |
| | | 4 | RX | Serial3 TX |
| | | 5 | PPS (optional) | D32 |
| | | 6 | 3V3 | Not connected |
| LCD 20x4 | JST-XH-4 | 1 | +5 V | 5V BUCK |
| | | 2 | GND | BATT- |
| | | 3 | SDA | SDA |
| | | 4 | SCL | SCL |
| E-stop | Panel mount | 1 | EN inhibit | Wired in series with driver EN |
| | | 2 | Sense | D31 |
| Battery sense | PCB screw | 1 | Divider tap | A2 |
| | | 2 | GND | BATT- |

## Notes

1. Keep the UART bus between the GIGA and both TMC2209 drivers under 20 cm and add 100 Ω series resistors if ringing is observed.
2. Route motor phases through shielded cable or twisted pairs; terminate shields at the enclosure.
3. The emergency stop switch must hard-open the enable lines and feed into D31 for software awareness.
4. Install ferrite beads on the GPS and joystick leads if operating in high EMI environments.
5. Include TVS diodes (SMBJ15A) across the 12 V rail for surge protection.

## Schematic Export Workflow

1. Import the connection tables into a schematic tool.
2. Map each connector to its mating harness.
3. Annotate wire colors and lengths for the loom drawing.
4. Export PDF and keep a copy in `docs/docs_assets/` for manufacturing release.
