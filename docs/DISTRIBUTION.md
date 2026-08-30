# Distribution

AXIOM is distributed for Windows in two forms.

## Installer

The recommended package for ordinary users is:

`AXIOM-v2.6.3-Windows-x64-Setup.exe`

It installs the application for the current user and creates a Start menu entry.

## Portable

The portable package is:

`AXIOM-v2.6.3-Windows-x64-Portable.zip`

Extract the complete archive and run `AxiomTelemetryWorkbench.exe`.

The executable is dynamically linked to Qt. The portable directory therefore includes the Qt runtime libraries, QML modules, platform plugins, and MinGW runtime required by the application.

Qt Creator and the Qt SDK are not required on a computer that is only running the packaged application.

## Source code

The Git repository contains AXIOM source code. The Qt SDK itself is intentionally not committed to the repository.