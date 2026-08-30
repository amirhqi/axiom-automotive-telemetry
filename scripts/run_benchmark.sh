#!/usr/bin/env bash
set -euo pipefail
BUILD_DIR="${1:-build-benchmark}"
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_BENCHMARKS=ON
cmake --build "$BUILD_DIR" --parallel --target ParserBenchmark
"$BUILD_DIR/ParserBenchmark" -iterations 10
