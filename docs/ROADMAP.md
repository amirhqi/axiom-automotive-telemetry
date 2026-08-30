# Roadmap

The roadmap favors deeper automotive/embedded engineering before adding unrelated cloud or visualization features.

## Near term

- Native Linux build validation with GCC
- Repeat automated tests on Linux
- Improve packaging/deployment documentation
- Record/replay workflow for telemetry sessions
- Additional performance measurements with reproducible test conditions

## Automotive transport

- SocketCAN transport on Linux
- Virtual CAN (`vcan`) development setup
- CAN frame monitor/filtering
- DBC-based signal decoding
- ISO-TP transport
- OBD-II live data and diagnostic trouble code experiments

## Embedded Linux

- Real ARM64 cross-build validation
- Deployment to an ARM Linux target
- Yocto/BitBake recipe validation against a reproducible image
- Startup/service integration appropriate to the target

## Out of scope for the current milestone

Kafka, machine learning and 3D visualization are intentionally not part of the core roadmap until automotive transport and embedded deployment are demonstrated cleanly.