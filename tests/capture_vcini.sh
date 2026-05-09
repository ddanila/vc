#!/bin/bash
# Boot VC under real DOS in QEMU, drive it to save VC.INI via
# Shift-F9, then mcopy the resulting VC.INI back to the host. We use
# this captured VC.INI as a "good" setup fixture for VC 4.99.09 under
# kvikdos — without it, VC paints the "Can't read disk" Error dialog
# at startup, presumably because some uninitialized state goes via
# the panel-redraw path that ends in PathEr=1.
#
# Usage: bash tests/capture_vcini.sh <version> [<output_path>]
#   version:     4.05 | 4.99.09
#   output_path: defaults to tests/fixtures/VC.<VERSION>.INI
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VERSION="${1:-4.99.09}"
OUT_FILE="${2:-$REPO_ROOT/tests/fixtures/VC.$VERSION.INI}"

VC_BINARY="$REPO_ROOT/build/$VERSION/VC.COM"
VC_OVL="$REPO_ROOT/build/$VERSION/VC.OVL"
CACHE="$REPO_ROOT/build/.cache"
BASE_IMG="$CACHE/floppy-minimal.img"

[[ -f "$VC_BINARY" ]] || { echo "ERROR: $VC_BINARY missing — run ./build.sh $VERSION first" >&2; exit 1; }
[[ -f "$BASE_IMG" ]] || { echo "ERROR: $BASE_IMG missing — run ./build.sh $VERSION first" >&2; exit 1; }

OUT_DIR="$REPO_ROOT/build/$VERSION/stage/inicap"
mkdir -p "$OUT_DIR"
BOOT_IMG="$OUT_DIR/boot.img"
QMP_SOCK="$OUT_DIR/qmp.sock"

QEMU_PID=""
cleanup() {
    if [[ -n "$QEMU_PID" ]]; then
        kill "$QEMU_PID" 2>/dev/null || true
        wait "$QEMU_PID" 2>/dev/null || true
    fi
    rm -f "$QMP_SOCK"
}
trap cleanup EXIT

cp "$BASE_IMG" "$BOOT_IMG"
mcopy -o -i "$BOOT_IMG" "$VC_BINARY" ::VC.COM
[[ -f "$VC_OVL" ]] && mcopy -o -i "$BOOT_IMG" "$VC_OVL" ::VC.OVL || true
printf '@ECHO OFF\r\nVC.COM\r\n' > "$OUT_DIR/AUTOEXEC.BAT"
mcopy -o -i "$BOOT_IMG" "$OUT_DIR/AUTOEXEC.BAT" ::AUTOEXEC.BAT

echo "==> booting VC $VERSION under QEMU to capture VC.INI"
rm -f "$QMP_SOCK"
timeout 90 qemu-system-i386 \
    -display none \
    -monitor none \
    -serial none \
    -drive if=floppy,index=0,format=raw,file="$BOOT_IMG" \
    -boot a \
    -m 4 \
    -qmp unix:"$QMP_SOCK",server,nowait \
    >/dev/null 2>/dev/null &
QEMU_PID=$!

# Wait for QMP socket.
for _ in $(seq 1 50); do
    [[ -S "$QMP_SOCK" ]] && break
    sleep 0.2
done
[[ -S "$QMP_SOCK" ]] || { echo "ERROR: QMP socket did not appear" >&2; exit 1; }

# Drive VC: wait for the F-key bar to render, send Shift-F9 to save
# setup, accept default filename with Enter, then F10 + Enter to quit.
python3 - "$QMP_SOCK" <<'PY'
import json, socket, sys, time

class QMP:
    def __init__(self, p):
        self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.s.settimeout(10)
        for _ in range(20):
            try:
                self.s.connect(p); break
            except (FileNotFoundError, ConnectionRefusedError):
                time.sleep(0.5)
        self._buf = b""
        self._recv_json()  # greeting
        self._send({"execute": "qmp_capabilities"}); self._recv_json()
    def _send(self, o): self.s.sendall(json.dumps(o).encode() + b"\n")
    def _recv_json(self):
        while True:
            i = self._buf.find(b"\n")
            if i < 0:
                self._buf += self.s.recv(65536); continue
            line = self._buf[:i]; self._buf = self._buf[i+1:]
            if line.strip():
                try: return json.loads(line)
                except json.JSONDecodeError: pass
    def cmd(self, c, **a):
        self._send({"execute": c, "arguments": a})
        while True:
            r = self._recv_json()
            if "return" in r or "error" in r: return r
    def key(self, *codes):
        keys = []
        for c in codes:
            keys.append({"type": "qcode", "data": c})
        self.cmd("send-key", keys=keys)
        time.sleep(0.1)
    def screen(self, path):
        self.cmd("human-monitor-command",
                 **{"command-line": f'pmemsave 0xb8000 4000 "{path}"'})
        with open(path, "rb") as f: raw = f.read(4000)
        return "\n".join(
            bytes(raw[r*160 + 2*c] for c in range(80))
                .decode("cp437", errors="replace").rstrip()
            for r in range(25))

q = QMP(sys.argv[1])
import tempfile, os
with tempfile.NamedTemporaryFile(suffix=".bin", delete=False) as f:
    tmp = f.name

# Wait until VC's F-key bar is on screen.
for _ in range(120):
    s = q.screen(tmp)
    if "Help" in s and "Quit" in s:
        break
    time.sleep(0.5)
else:
    print("WARN: VC didn't paint F-key bar in time", flush=True)

# A few extra ticks for VC to finish initial render / dismiss any banner.
time.sleep(1.0)

# Send Shift-F9 (save setup).
q.key("shift", "f9")
time.sleep(2.0)

# Confirm "Save" dialog with Enter (default button).
q.key("ret")
time.sleep(2.0)

# Quit with F10 (Quit).
q.key("f10")
time.sleep(1.0)

# Confirm with Enter (Yes/OK button).
q.key("ret")
time.sleep(2.0)

print("--- final screen ---")
print(q.screen(tmp))
os.unlink(tmp)
PY

# Wait briefly for QEMU to exit (VC.COM exited → AUTOEXEC.BAT done → DOS prompt).
sleep 2

echo "==> extracting VC.INI from the floppy"
mcopy -o -i "$BOOT_IMG" ::VC.INI "$OUT_FILE.tmp" 2>/dev/null || {
    echo "ERROR: VC.INI not found on the floppy after Shift-F9" >&2
    exit 1
}
mv "$OUT_FILE.tmp" "$OUT_FILE"
echo "==> wrote $(wc -c <"$OUT_FILE") bytes to $OUT_FILE"
