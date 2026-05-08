#!/usr/bin/env python3
"""Minimal QEMU text-mode screen matcher for DOS E2E tests.

Adapted from the MS-DOS repo test harness. Connects to QEMU via QMP, reads
text-mode video memory, waits for one or more screen patterns, optionally sends
keystrokes, then writes the matched and final screens to a log.
"""

import json
import os
import socket
import sys
import tempfile
import time


VRAM_PHYS = 0xB8000
VRAM_SIZE = 4000
COLS = 80
ROWS = 25
POLL_INTERVAL = 0.3
KEY_DELAY = 0.05
TIMEOUT = 120


class QMPConnection:
    def __init__(self, sock_path: str, retries: int = 10):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.settimeout(10.0)
        for attempt in range(retries):
            try:
                self.sock.connect(sock_path)
                break
            except (ConnectionRefusedError, FileNotFoundError):
                if attempt == retries - 1:
                    raise
                time.sleep(0.5)
        self._buf = b""
        self._recv_json()
        self._send({"execute": "qmp_capabilities"})
        self._recv_json()

    def _send(self, obj: dict) -> None:
        self.sock.sendall(json.dumps(obj).encode() + b"\n")

    def _recv_json(self) -> dict:
        while True:
            nl = self._buf.find(b"\n")
            if nl >= 0:
                line = self._buf[:nl]
                self._buf = self._buf[nl + 1 :]
                if line.strip():
                    try:
                        return json.loads(line)
                    except json.JSONDecodeError:
                        pass
            else:
                chunk = self.sock.recv(65536)
                if not chunk:
                    raise ConnectionError("QMP connection closed")
                self._buf += chunk

    def _recv_response(self) -> dict:
        while True:
            obj = self._recv_json()
            if "return" in obj or "error" in obj:
                return obj

    def human_cmd(self, cmd_line: str) -> str:
        self._send(
            {
                "execute": "human-monitor-command",
                "arguments": {"command-line": cmd_line},
            }
        )
        resp = self._recv_response()
        return resp.get("return", "")

    def send_key(self, qcode: str) -> None:
        self._send(
            {
                "execute": "send-key",
                "arguments": {"keys": [{"type": "qcode", "data": qcode}]},
            }
        )
        self._recv_response()

    def close(self) -> None:
        self.sock.close()


def read_screen_text(qmp: QMPConnection, tmp_path: str) -> str:
    qmp.human_cmd(f'pmemsave 0x{VRAM_PHYS:X} {VRAM_SIZE} "{tmp_path}"')
    try:
        with open(tmp_path, "rb") as f:
            raw = f.read(VRAM_SIZE)
    except FileNotFoundError:
        return ""
    if len(raw) < VRAM_SIZE:
        return ""

    chars = bytes(raw[i] for i in range(0, VRAM_SIZE, 2))
    lines = []
    for row in range(ROWS):
        line = chars[row * COLS : (row + 1) * COLS]
        lines.append(line.decode("cp437", errors="replace").rstrip())
    return "\n".join(lines)


def send_keys(qmp: QMPConnection, keys_str: str) -> None:
    if not keys_str:
        return
    for key in keys_str.split("+"):
        key = key.strip()
        if not key:
            continue
        qmp.send_key(key)
        time.sleep(KEY_DELAY)


def main() -> None:
    args = sys.argv[1:]
    if len(args) < 4 or len(args) % 2 != 0:
        sys.exit("usage: screen_expect.py qmp_sock screen_log [pattern response]...")

    qmp_sock = args[0]
    log_path = args[1]
    rules = [(args[i], args[i + 1]) for i in range(2, len(args), 2)]

    print(f"screen_expect: {len(rules)} rules, connecting to {qmp_sock}", flush=True)

    qmp = QMPConnection(qmp_sock)
    rule_idx = 0
    deadline = time.monotonic() + TIMEOUT

    with tempfile.NamedTemporaryFile(suffix=".bin", delete=False) as tmp:
        tmp_path = tmp.name

    log = open(log_path, "w")
    try:
        while rule_idx < len(rules) and time.monotonic() < deadline:
            screen = read_screen_text(qmp, tmp_path)
            if not screen:
                time.sleep(POLL_INTERVAL)
                continue

            pattern, response = rules[rule_idx]
            if pattern in screen:
                log.write(f"=== Rule {rule_idx}: matched '{pattern}' ===\n")
                log.write(screen + "\n\n")
                print(f"  rule[{rule_idx}] matched: {pattern[:60]}", flush=True)
                send_keys(qmp, response)
                rule_idx += 1
                time.sleep(0.5)
            else:
                time.sleep(POLL_INTERVAL)

        time.sleep(1.0)
        screen = read_screen_text(qmp, tmp_path)
        log.write("=== Final screen ===\n")
        log.write(screen + "\n")

        if rule_idx < len(rules):
            log.write(f"\nTIMEOUT: only {rule_idx}/{len(rules)} rules matched\n")
            print(
                f"screen_expect: TIMEOUT after {TIMEOUT}s - "
                f"{rule_idx}/{len(rules)} rules matched",
                flush=True,
            )
            sys.exit(1)

        print(f"screen_expect: all {len(rules)} rules matched.", flush=True)
    finally:
        log.close()
        os.unlink(tmp_path)
        qmp.close()


if __name__ == "__main__":
    main()
