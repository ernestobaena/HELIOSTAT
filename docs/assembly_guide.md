# Heliostat Assembly, Wiring, and Calibration Guide

This guide walks through the full assembly of the heliostat subsystem, including wiring, limit switch setup, homing, calibration, and target-setting workflows. The process assumes reference to the wiring schematic (`wiring_schematic.pdf` / `.svg`) and mechanical drawings generated in this repository.

## 1. Preparation

1. Review the mechanical drawings for the mirror carrier, elevation bracket, azimuth base, belt guard, and limit switch mounts. Verify hardware availability and finish requirements.
2. Inspect all safety interlock devices (emergency stops, door switches, over-current modules) for continuity.
3. Stage harnesses for power (24 VDC), control (I2C/UART), and signal (limit switches, encoders, joystick).
4. Print the wiring schematic to keep track of the harness routing.

## 2. Mechanical Assembly

1. **Azimuth Base**
   - Anchor the azimuth base plate to the foundation using Ø14 mm anchors on the 200 mm bolt circle.
   - Install the slew bearing and verify free rotation.
2. **Elevation Bracket**
   - Bolt the elevation bracket to the azimuth yoke, ensuring the Ø25 mm pivot boss seats fully.
   - Add gussets and torque the hardware to specification.
3. **Mirror Carrier**
   - Attach the mirror carrier to the elevation bracket using Ø8 hardware through the slotted holes to allow tip/tilt adjustment.
   - Bond or fasten stiffening ribs to maintain mirror flatness (<0.5 mm deviation).
4. **Belt Guard and Limit Switch Mounts**
   - Install the belt guard after verifying belt alignment, maintaining 20 mm clearance throughout travel.
   - Mount the limit switch plates so that the roller levers engage before mechanical stops in both azimuth and elevation.

## 3. Wiring

Refer to `wiring_schematic.pdf` for the connection overview.

1. Route the 24 VDC supply from the solar combiner/battery pack into the safety interlock relay block.
2. Daisy-chain the 24 V bus from the interlocks to the azimuth and elevation motor drivers.
3. Run the controller harness:
   - UART pair to the GPS receiver.
   - I2C twisted pair to the LCD status panel.
   - Analog shielded cable to the joystick potentiometers; discrete line to the pushbutton.
4. Terminate limit switch leads back to the MCU GPIO, observing shield drain connection at the controller end.
5. Connect encoder feedback pairs from each motor to the driver stack, with telemetry return to the controller as required.
6. Confirm chassis ground bonding between motor frames, control enclosure, and solar negative reference.

## 4. Safety Interlocks and Power-Up

1. Wire the emergency stop, door switches, and overcurrent sensors in series inside the interlock block.
2. Verify interlock drop-out removes power from both motor drivers before energizing the system.
3. Perform insulation resistance tests on all power runs prior to live connection.
4. Apply power with the motors mechanically decoupled and observe controller boot diagnostics on the LCD panel.

## 5. Homing Procedure

1. With the joystick disabled, command a low-speed homing sequence from the controller interface.
2. Observe azimuth rotation until the AZ- limit switch engages; confirm motor torque is removed.
3. Repeat for the elevation axis, ensuring EL- engages cleanly without contacting mechanical stops.
4. Jog away from each limit and verify both positive limits trigger in the correct travel direction.
5. Record raw encoder counts at each limit for reference.

## 6. Calibration

1. Level the azimuth base using a machinist level; zero azimuth encoder counts once level.
2. Use a calibrated inclinometer on the mirror carrier to set 0° elevation; update the elevation offset in controller firmware.
3. Align the mirror normal with a known terrestrial target at solar noon; record azimuth/elevation offsets.
4. Enter calibration offsets in the controller configuration file and store to non-volatile memory.

## 7. Target-Setting Workflow

1. Obtain site coordinates from the GPS receiver and confirm they are reflected in controller logs.
2. Input the desired target coordinates (azimuth/elevation or celestial coordinates) into the control UI.
3. Validate sun-tracking predictions against astronomical tables; adjust for atmospheric refraction if required.
4. Run a tracking simulation with motors disabled to ensure commanded positions match expectations.
5. Enable tracking mode and monitor current draw, limit switch state, and LCD telemetry during the first full cycle.

## 8. Documentation and Sign-Off

1. Capture photographs of completed wiring harnesses, interlock routing, and limit switch positions.
2. Update the maintenance log with calibration offsets, encoder counts, and interlock test results.
3. Archive the signed assembly checklist, wiring schematic markups, and calibration worksheet within the project documentation vault.

Refer to the `docs/` directory for the full set of drawings, schematics, and generated SVG sources should adjustments be required.
