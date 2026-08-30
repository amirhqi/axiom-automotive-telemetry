#!/usr/bin/env python3
"""TCP telemetry simulator with fault injection and disconnect testing."""

from __future__ import annotations

import argparse
import math
import random
import socket
import time


def good_packet(t: float) -> str:
    rpm = int(2200 + 900 * math.sin(t * 1.3) + random.randint(-40, 40))
    speed = max(0, int(75 + 20 * math.sin(t * 0.45)))
    temp = 88.0 + 4.0 * math.sin(t * 0.12)
    battery = 13.8 + random.uniform(-0.12, 0.12)
    return f"RPM={rpm},SPEED={speed},TEMP={temp:.1f},BATTERY={battery:.2f}\n"


def bad_packet() -> str:
    candidates = [
        "RPM=abc,SPEED=82,TEMP=91.0,BATTERY=13.8\n",
        "RPM=2400,SPEED=82,TEMP=91.0\n",
        "RPM=20000,SPEED=82,TEMP=91.0,BATTERY=13.8\n",
        "RPM=2400,SPEED,TEMP=91.0,BATTERY=13.8\n",
        "RPM=2400,RPM=2500,SPEED=82,TEMP=91.0,BATTERY=13.8\n",
    ]
    return random.choice(candidates)


def send_maybe_split(client: socket.socket, payload: bytes, split_rate: float) -> None:
    if len(payload) > 6 and random.random() < split_rate:
        cut = random.randint(2, len(payload) - 2)
        client.sendall(payload[:cut])
        time.sleep(0.01)
        client.sendall(payload[cut:])
    else:
        client.sendall(payload)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Automotive telemetry TCP simulator")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=5000)
    parser.add_argument("--rate", type=float, default=10.0, help="Packets per second")
    parser.add_argument("--fault-rate", type=float, default=0.0, help="Malformed packet probability [0..1]")
    parser.add_argument("--split-rate", type=float, default=0.15, help="Probability of splitting one frame across TCP writes")
    parser.add_argument("--disconnect-every", type=float, default=0.0, help="Force disconnect every N seconds (0 disables)")
    parser.add_argument("--seed", type=int, default=None)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.rate <= 0:
        raise SystemExit("--rate must be > 0")
    if not 0 <= args.fault_rate <= 1 or not 0 <= args.split_rate <= 1:
        raise SystemExit("rates must be between 0 and 1")

    random.seed(args.seed)
    interval = 1.0 / args.rate

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((args.host, args.port))
        server.listen(1)
        print(f"[sim] listening on {args.host}:{args.port} @ {args.rate:g} pkt/s")
        print(f"[sim] fault-rate={args.fault_rate:g}, split-rate={args.split_rate:g}, disconnect-every={args.disconnect_every:g}s")

        while True:
            client, address = server.accept()
            print(f"[sim] client connected: {address}")
            with client:
                start = time.monotonic()
                session_start = start
                sent = 0
                try:
                    while True:
                        now = time.monotonic()
                        if args.disconnect_every > 0 and now - session_start >= args.disconnect_every:
                            print("[sim] intentional disconnect for reconnect test")
                            break

                        payload = bad_packet() if random.random() < args.fault_rate else good_packet(now - start)
                        send_maybe_split(client, payload.encode("utf-8"), args.split_rate)
                        sent += 1
                        if sent % max(1, int(args.rate * 5)) == 0:
                            print(f"[sim] sent={sent}")
                        time.sleep(interval)
                except (BrokenPipeError, ConnectionResetError, ConnectionAbortedError):
                    print("[sim] client disconnected")


if __name__ == "__main__":
    main()
