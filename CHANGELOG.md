# Changelog

All notable changes to AXIOM will be documented here.

The project follows semantic-style release tags where practical.

## [2.6.0] - 2026-08-30

### Added
- Industrial workstation UI with Live Monitor, Trace and Diagnostics workspaces
- TCP, UDP and Serial telemetry acquisition paths
- Dedicated I/O worker thread
- Buffered TCP frame handling
- Automatic reconnect behavior
- Bounded telemetry history
- CSV session logging
- Python telemetry/fault simulator
- Unit, TCP integration, UDP integration, logger and functional smoke tests
- AXIOM product identity and desktop application icon
- GitHub repository documentation and CI configuration

### Validated
- Local Windows 11 build with Qt 6.11.1 / MinGW-w64 13.1
- Five automated test targets passing locally