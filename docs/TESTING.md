# Testing

AXIOM uses multiple levels of automated testing because a telemetry application can fail at parsing, transport or end-to-end integration boundaries.

## Test levels

### Unit tests
Packet parsing and validation are tested independently from sockets and UI state.

### TCP integration tests
A local TCP server sends complete, malformed and deliberately fragmented application frames. The tests verify that stream buffering reconstructs frames before parsing.

### UDP integration tests
Local datagrams exercise the UDP receive path and malformed-frame rejection.

### CSV logger tests
Session logging is checked separately from acquisition.

### Functional smoke test
A local server drives the application backend through a small end-to-end path and verifies controller updates.

## Running tests

```powershell
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Manual scenarios

The Python simulator supports additional manual stress/fault scenarios:

```powershell
python .\tools\simulator.py --fault-rate 0.05
python .\tools\simulator.py --split-rate 0.8
python .\tools\simulator.py --disconnect-every 10
python .\tools\simulator.py --rate 500 --fault-rate 0.02
```

When reporting performance numbers, record the machine, build type, sample rate and duration. Do not publish unmeasured throughput claims.