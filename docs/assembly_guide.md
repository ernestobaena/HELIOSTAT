# Assembly & Calibration Guide

## 1. Pre-assembly Checklist

- Verify all parts listed in `BOM.csv` are on hand.
- Pre-program the Arduino GIGA R1 WiFi with the firmware in `src/`.
- Charge the LiFePO4 battery to at least 50% SOC.
- Inspect the mirror carrier and brackets for burrs; deburr if necessary.

## 2. Mechanical Assembly

1. **Azimuth Base** – Mount the lazy-Susan bearing to the base plate using M6 countersunk screws, threadlocker medium applied. Attach the azimuth stepper bracket and install the worm gearbox or belt reduction.
2. **Elevation Frame** – Bolt the elevation bracket to the azimuth base. Install the elevation worm gearbox, ensuring the motor wiring exits toward the rear.
3. **Mirror Carrier** – Install PEM studs and corner bumpers. Attach the mirror with silicone pads and compression clips.
4. **Limit Switches** – Mount roller lever switches to the provided bosses. Adjust the actuator flags so the home switch triggers before the mechanical hard stop.

## 3. Electrical Wiring

1. Route the motor phase leads through cable chain and terminate at the TMC2209 drivers.
2. Wire the limit switches to the controller harness using shielded cable with drain connected at the enclosure.
3. Connect the joystick, button panel, LCD, and GPS to the GIGA according to `wiring_overview.md`.
4. Install the emergency stop in line with the driver enable signals and connect its sense contact to D31.
5. Tie the battery, MPPT, and DC/DC converters to the fused distribution block. Observe polarity at all times.

## 4. Initial Power-up

1. With motors disconnected, power the control enclosure from a current-limited supply (12 V / 1 A) to verify no shorts.
2. Connect the LiFePO4 battery and solar input. Confirm the buck converters output 5 V.
3. Power the system, allow the GIGA to boot, and ensure the LCD displays `BOOT` followed by `HOMING`.

## 5. Homing & Calibration

1. Engage the homing sequence; verify both axes reverse toward their home switches and stop cleanly.
2. After homing completes, enter manual mode and adjust azimuth/elevation offsets in `config.hpp` if the mirror zero is misaligned.
3. Aim the mirror at a distant target using the joystick and press **Set Target** to store the target vector.
4. Verify persistence by power-cycling and ensuring the stored target reloads (`TRACK` resumes after GPS lock).

## 6. Safety Checks

- Press the emergency stop; confirm motors de-energize and the LCD reports `E-STOP`.
- Force a limit switch while moving manually to ensure motion halts immediately.
- Disconnect the GPS antenna and verify the UI reports a stale fix warning.

## 7. Field Commissioning

1. Level the tripod or pier using integrated leveling screws.
2. Align the azimuth home position with true North using a GPS two-point method.
3. Run a full day tracking test, observing the reflected spot at a 10 m target every hour.
4. Confirm the system parks after sunset (sun altitude < −2°) and stows after timeout.

## 8. Maintenance Tips

- Inspect belt tension and worm gear backlash monthly.
- Reapply silicone sealant to the enclosure glands annually.
- Record any faults in the maintenance log stored alongside the controller.
