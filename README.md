# AXIOM Automotive Telemetry Workbench

**Real-time telemetry acquisition, trace inspection, diagnostics, logging, and transport testing in C++17 / Qt 6.**

[![CI](https://github.com/amirhqi/axiom-automotive-telemetry/actions/workflows/ci.yml/badge.svg)](https://github.com/amirhqi/axiom-automotive-telemetry/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/amirhqi/axiom-automotive-telemetry?display_name=tag)](https://github.com/amirhqi/axiom-automotive-telemetry/releases/latest)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C)
![Qt 6](https://img.shields.io/badge/Qt-6-41CD52)
![Windows](https://img.shields.io/badge/Windows-11-0078D4)
![Linux](https://img.shields.io/badge/Linux-target-FCC624)

## Preview

### Live Monitor

![AXIOM Live Monitor](docs/images/live-monitor.png)

### Trace

![AXIOM Trace](docs/images/trace.png)

### Diagnostics

![AXIOM Diagnostics](docs/images/diagnostics.png)

## Overview

AXIOM is an engineering telemetry and diagnostics workbench built around the same software concerns found in hardware-facing and automotive applications: asynchronous I/O, stream framing, validation, reconnect behavior, diagnostics, logging, responsive UI design, and automated verification.

**Acquire -> Validate -> Observe -> Diagnose**

## Core Capabilities

- C++17 backend
- Qt 6 / Qt Quick / QML interface
- TCP, UDP, and Serial transports
- dedicated worker-thread I/O architecture
- TCP fragmentation-safe receive buffering
- frame validation and rejection counters
- automatic reconnect behavior
- live telemetry visualization
- raw frame trace
- diagnostics and session statistics
- CSV logging
- deterministic fault-injection simulator
- unit, integration, and functional smoke tests
- CMake / Ninja workflow
- Embedded Linux / ARM64 / Yocto scaffolding

## Architecture

```mermaid
flowchart LR
    ECU[ECU / MCU / Simulator] -->|TCP / UDP / Serial| Worker[TelemetryWorker]
    Worker --> Buffer[Receive Buffer]
    Buffer --> Parser[PacketParser]
    Parser --> Controller[TelemetryController]
    Controller --> UI[Qt Quick / QML]
    Controller --> Logger[CSV Logger]
```

The Qt GUI remains on the main thread. Transport and connection work are handled separately so I/O activity does not block rendering and interaction.

See [Architecture](docs/ARCHITECTURE.md) and [Design Decisions](docs/DESIGN_DECISIONS.md).

## Download

### Windows Installer — recommended

Download **AXIOM-v2.6.1-Windows-x64-Setup.exe** from the [latest release](https://github.com/amirhqi/axiom-automotive-telemetry/releases/latest).

Run the installer and launch AXIOM from the Start menu or desktop shortcut.

### Windows Portable

Download **AXIOM-v2.6.1-Windows-x64-Portable.zip** from the [latest release](https://github.com/amirhqi/axiom-automotive-telemetry/releases/latest).

Extract the complete archive and run:

```text
AxiomTelemetryWorkbench.exe
```

Qt Creator and the Qt SDK are **not required** to run either Windows package.

## Quick Demo

Run:

```bash
python tools/simulator.py
```

Then connect AXIOM to:

```text
Transport: TCP
Host:      127.0.0.1
Port:      5000
```

Fault scenarios:

```bash
python tools/simulator.py --fault-rate 0.15
python tools/simulator.py --split-rate 0.8
python tools/simulator.py --disconnect-every 5
python tools/simulator.py --rate 500 --fault-rate 0.02
```

## Automated Tests

The repository includes:

- `PacketParserTests`
- `TcpIntegrationTests`
- `UdpIntegrationTests`
- `CsvLoggerTests`
- `FunctionalSmokeTests`

Validated local Windows run: **5/5 test targets passed.**

```bash
ctest --test-dir build --output-on-failure
```

## Build from Source

Requirements:

- Qt 6.11.x
- a C++17 compiler
- CMake
- Ninja or another supported CMake generator

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

See [Building](docs/BUILDING.md).

## Repository Structure

```text
.github/        CI and repository templates
assets/         application branding assets
benchmarks/     benchmark code
cmake/          toolchain files
docs/           architecture, protocol, build, test and distribution docs
qml/            Qt Quick UI
scripts/        build and test helpers
src/            C++ source
tests/          automated tests
tools/          simulators and utilities
yocto/          Embedded Linux / Yocto scaffold
```

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

## Scope

AXIOM is an engineering portfolio and experimental telemetry project. It is **not** a safety-certified ECU, ISO 26262 component, or certified production-vehicle diagnostic product.

## Author

**Amirhossein Haghi**

## License

See [LICENSE](LICENSE).