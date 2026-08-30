#!/usr/bin/env python3
"""UDP telemetry simulator for Automotive Telemetry Console V2.5."""
import argparse
import math
import random
import socket
import time


def frame(t: float) -> str:
    rpm = int(1800 + 900 * (1 + math.sin(t * 1.7)))
    speed = int(45 + 35 * (1 + math.sin(t * 0.45)))
    temp = 82.0 + 8.0 * math.sin(t * 0.12)
    battery = 13.7 + 0.25 * math.sin(t * 0.31)
    return f"RPM={rpm},SPEED={speed},TEMP={temp:.1f},BATTERY={battery:.2f}\n"


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument('--host', default='127.0.0.1')
    p.add_argument('--port', type=int, default=5001)
    p.add_argument('--rate', type=float, default=10.0)
    p.add_argument('--fault-rate', type=float, default=0.0)
    args = p.parse_args()
    delay = 1.0 / max(args.rate, 0.1)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    start = time.monotonic()
    print(f"Sending UDP telemetry to {args.host}:{args.port} at {args.rate:g} pkt/s")
    try:
        while True:
            payload = frame(time.monotonic() - start)
            if random.random() < args.fault_rate:
                payload = 'RPM=bad,SPEED=80,TEMP=90,BATTERY=13.8\n'
            sock.sendto(payload.encode(), (args.host, args.port))
            time.sleep(delay)
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
