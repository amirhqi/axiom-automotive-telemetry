#!/usr/bin/env bash
set -euo pipefail
BUILD_DIR="${1:-build}"
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build "$BUILD_DIR" --parallel
