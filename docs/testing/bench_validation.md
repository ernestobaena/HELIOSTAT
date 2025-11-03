# Bench Validation Checklist

| Step | Description | Pass/Fail | Notes |
| --- | --- | --- | --- |
| 1 | Visual inspection of PCB harness and connectors |  |  |
| 2 | Continuity test between supply rails and ground |  |  |
| 3 | Power-on with motors disconnected (0.5 A limit) |  |  |
| 4 | Verify 5 V rail within 4.9–5.1 V |  |  |
| 5 | Connect motors, confirm enable toggles via firmware |  |  |
| 6 | Run homing routine without mirror load |  |  |
| 7 | Trigger each limit switch manually, observe halt |  |  |
| 8 | Simulate emergency stop, ensure EN line drops |  |  |
| 9 | Inject synthetic GPS sentences for fixed position |  |  |
| 10 | Run two-minute tracking loop, observe stepper activity |  |  |

Record anomalies in `docs/testing/test_log.md`.
