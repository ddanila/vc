#!/bin/bash
# QEMU smoke test: boot a built VC.COM under real DOS in qemu-system-i386
# and verify the panel renders. Uses the same floppy image as build.sh
# (build/.cache/floppy-minimal.img).
#
# Usage: test_volkov_e2e.sh <version>
#   version: 4.05 | 4.99.09
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

VERSION="${1:-${VC_E2E_VERSION:-4.05}}"
VC_BINARY="$REPO_ROOT/build/$VERSION/VC.COM"

if [[ ! -f "$VC_BINARY" ]]; then
    echo "ERROR: $VC_BINARY not found — run ./build.sh $VERSION first" >&2
    exit 1
fi

CACHE="$REPO_ROOT/build/.cache"
BASE_IMG="$CACHE/floppy-minimal.img"
DOS_IMAGE_URL="${DOS_IMAGE_URL:-https://github.com/ddanila/msdos/releases/download/0.1/floppy-minimal.img}"

OUT_DIR="$REPO_ROOT/build/$VERSION/stage/e2e"
mkdir -p "$OUT_DIR"
BOOT_IMG="$OUT_DIR/volkov-e2e.img"
AUTOEXEC="$OUT_DIR/AUTOEXEC.BAT"
QMP_SOCK="$OUT_DIR/volkov-e2e-qmp.sock"
SCREEN_LOG="$OUT_DIR/volkov-e2e-screen.log"

PASS=0
FAIL=0
QEMU_PID=""

ok()   { echo "  PASS: $1"; PASS=$((PASS + 1)); }
fail() { echo "  FAIL: $1"; FAIL=$((FAIL + 1)); }

cleanup() {
    if [[ -n "$QEMU_PID" ]]; then
        kill "$QEMU_PID" 2>/dev/null || true
        wait "$QEMU_PID" 2>/dev/null || true
    fi
    rm -f "$QMP_SOCK" "$AUTOEXEC"
}

trap cleanup EXIT

if [[ ! -f "$BASE_IMG" ]]; then
    echo "Downloading minimal DOS floppy from $DOS_IMAGE_URL ..."
    mkdir -p "$CACHE"
    python3 - "$DOS_IMAGE_URL" "$BASE_IMG" <<'PY'
import shutil, sys, urllib.request
url, dest = sys.argv[1], sys.argv[2]
with urllib.request.urlopen(url) as r, open(dest, "wb") as out:
    shutil.copyfileobj(r, out)
PY
fi

echo "=== Volkov Commander DOS e2e ($VERSION) ==="
echo "VC.COM: $VC_BINARY"

cp "$BASE_IMG" "$BOOT_IMG"
mcopy -o -i "$BOOT_IMG" "$VC_BINARY" ::VC.COM
# 4.99.09 also needs VC.OVL on the boot floppy.
if [[ "$VERSION" == "4.99.09" ]]; then
    if [[ -f "$REPO_ROOT/build/$VERSION/VC.OVL" ]]; then
        mcopy -o -i "$BOOT_IMG" "$REPO_ROOT/build/$VERSION/VC.OVL" ::VC.OVL
    else
        echo "ERROR: build/$VERSION/VC.OVL missing — 4.99.09 needs it" >&2
        exit 1
    fi
fi
printf '@ECHO OFF\r\nVC.COM\r\n' > "$AUTOEXEC"
mcopy -o -i "$BOOT_IMG" "$AUTOEXEC" ::AUTOEXEC.BAT

echo "Booting QEMU ..."
rm -f "$QMP_SOCK" "$SCREEN_LOG"
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

for _ in $(seq 1 50); do
    [[ -S "$QMP_SOCK" ]] && break
    sleep 0.2
done

if [[ ! -S "$QMP_SOCK" ]]; then
    echo "ERROR: QMP socket did not appear"
    exit 1
fi

echo "Waiting for Volkov Commander screen ..."
# The function-key bar text and version-line banner differ across
# 4.05 and 4.99.09; match on substrings present in both.
python3 "$REPO_ROOT/tests/screen_expect.py" \
    "$QMP_SOCK" "$SCREEN_LOG" \
    'Help' '' \
    'View' '' \
    'Quit' ''

kill "$QEMU_PID" 2>/dev/null || true
wait "$QEMU_PID" 2>/dev/null || true
QEMU_PID=""

echo ""
echo "--- Volkov Commander checks ($VERSION) ---"

if [[ -s "$SCREEN_LOG" ]]; then
    ok "Screen log created"
else
    fail "Screen log missing or empty"
fi

if grep -q "Volkov Commander" "$SCREEN_LOG"; then
    ok "Volkov Commander banner rendered"
else
    fail "Volkov Commander banner not found"
fi

if grep -q "Help" "$SCREEN_LOG" && grep -q "Quit" "$SCREEN_LOG"; then
    ok "Function-key bar rendered (Help…Quit)"
else
    fail "Function-key bar incomplete"
fi

if grep -qi "vc *com" "$SCREEN_LOG"; then
    ok "Directory panel shows injected VC.COM"
else
    fail "Directory panel does not show VC.COM"
fi

if grep -q "=== Final screen ===" "$SCREEN_LOG"; then
    ok "Final screen captured"
else
    fail "Final screen not captured"
fi

if [[ $FAIL -gt 0 ]]; then
    echo ""
    echo "--- screen log ---"
    cat "$SCREEN_LOG" 2>/dev/null || true
    echo "--- end screen log ---"
fi

echo ""
echo "Results: $PASS passed, $FAIL failed"
[[ $FAIL -eq 0 ]]
