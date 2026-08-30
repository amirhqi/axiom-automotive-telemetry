#!/usr/bin/env python3
"""Linux/macOS pseudo-terminal telemetry writer. Uses only the Python stdlib."""

from __future__ import annotations

import argparse
import math
import random
import time


def packet(t: float) -> bytes:
    rpm = int(2100 + 800 * math.sin(t * 1.2))
    speed = max(0, int(70 + 18 * math.sin(t * 0.4)))
    temp = 87.0 + 3.0 * math.sin(t * 0.15)
    battery = 13.8 + random.uniform(-0.08, 0.08)
    return f"RPM={rpm},SPEED={speed},TEMP={temp:.1f},BATTERY={battery:.2f}\n".encode()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--device", required=True, help="PTY/device path, e.g. /dev/pts/4")
    parser.add_argument("--rate", type=float, default=10.0)
    args = parser.parse_args()

    interval = 1.0 / args.rate
    print(f"[serial-sim] writing to {args.device} @ {args.rate:g} pkt/s")
    with open(args.device, "wb", buffering=0) as device:
        start = time.monotonic()
        while True:
            device.write(packet(time.monotonic() - start))
            time.sleep(interval)


if __name__ == "__main__":
    main()
