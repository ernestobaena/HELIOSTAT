# Field Test Protocol

## Daily Tracking Trial

1. Install the heliostat outdoors with clear line of sight to the sun and target.
2. Perform homing and set the target vector using the joystick.
3. Start data logging via USB serial at 115200 baud (capture sun az/el, mirror az/el, battery voltage).
4. Observe beam placement on the target every hour; record offset in centimetres.
5. Note weather conditions (ambient temperature, wind speed, cloud cover).
6. At sunset, confirm the controller transitions to `PARK` then `STOW` within 10 minutes of sun altitude < −2°.
7. Review the log buffer (use serial command `LOG?`) and store results in `tests/dayXX_log.txt`.

## Fault Injection Tests

- **GPS Loss**: Disconnect antenna for 5 minutes. Expected behaviour: warning message, tracking continues on last solution, auto-recovery once signal returns.
- **Overcurrent Simulation**: Increase axis friction until driver reports stall; firmware should raise `FAULT` and halt.
- **Wind Gust**: Manually disturb mirror while tracking. Verify mount holds position and re-centres.

## Acceptance Criteria

- Beam radius ≤ 5 cm at 10 m target during steady sun.
- No missed steps or limit trips during daily operation.
- Battery voltage remains above 11.6 V throughout the day.
- System resumes autonomous tracking after power cycle without user intervention.
