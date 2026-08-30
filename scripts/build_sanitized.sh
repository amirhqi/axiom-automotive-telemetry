#!/usr/bin/env bash
set -euo pipefail
BUILD_DIR="${1:-build-sanitized}"
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DENABLE_SANITIZERS=ON
cmake --build "$BUILD_DIR" --parallel
ctest --test-dir "$BUILD_DIR" --output-on-failure
