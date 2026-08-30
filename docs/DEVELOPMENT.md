# Development guide

## Toolchain

The current Windows development configuration uses Qt 6.11.x, MinGW-w64, CMake and Ninja.

## Recommended workflow

1. Create a short-lived feature branch.
2. Make one focused change.
3. Build locally.
4. Run the automated tests.
5. Update documentation when behavior or architecture changes.
6. Open a pull request with the motivation, implementation notes and test evidence.

## Build quality

Prefer compiler warnings, debug builds and sanitizers where supported. Keep generated build directories outside source control.

## C++ style

- RAII for resource ownership
- Explicit ownership/lifetime
- `override` for overridden virtual functions
- `const` where it communicates intent
- Small focused classes/functions
- Avoid blocking operations on the GUI thread
- Prefer Qt signal/slot boundaries for cross-thread communication

The repository `.clang-format` and `.editorconfig` are the formatting source of truth.

## Commit messages

Use short imperative messages that describe the change, for example:

```text
feat: add UDP transport
fix: preserve fragmented TCP frames
 test: cover reconnect smoke path
 docs: document worker-thread model
```

Do not rewrite history merely to make a portfolio look older or larger than it is.