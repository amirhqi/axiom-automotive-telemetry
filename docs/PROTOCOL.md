# Telemetry Protocol

The current demonstration protocol is intentionally human-readable:

`	ext
RPM=2450,SPEED=82,TEMP=91.5,BATTERY=13.90
`

## Fields

- RPM: engine speed
- SPEED: vehicle speed
- TEMP: coolant temperature
- BATTERY: battery/system voltage

## Framing

TCP and Serial use newline-delimited application frames.

UDP datagrams may contain one or more newline-delimited frames.

## Validation

The parser rejects malformed values, missing fields and values outside the configured application ranges.

The next automotive protocol layer planned for the project is CAN/SocketCAN with DBC-driven signal decoding.