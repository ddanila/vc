#!/bin/bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="$REPO_ROOT/out"
BASE_IMG="${MSDOS_BASE_IMG:-$OUT_DIR/floppy-minimal.img}"
E2E_TARGET="${1:-${VC_E2E_TARGET:-asm}}"
case "$E2E_TARGET" in
    asm|original)
        E2E_NAME="asm"
        E2E_TITLE="byte-identical ASM VC.COM"
        E2E_BINARY="$REPO_ROOT/ORIG_VC/SRC/VCW.COM"
        ;;
    c|recreation|vc)
        E2E_NAME="c"
        E2E_TITLE="C recreation VC.COM"
        E2E_BINARY="$REPO_ROOT/src/vc.com"
        ;;
    *)
        echo "usage: $0 [asm|c]" >&2
        exit 2
        ;;
esac
BOOT_IMG="$OUT_DIR/volkov-e2e-$E2E_NAME.img"
AUTOEXEC="$OUT_DIR/AUTOEXEC-$E2E_NAME.BAT"
QMP_SOCK="$OUT_DIR/volkov-e2e-$E2E_NAME-qmp.sock"
SCREEN_LOG="$OUT_DIR/volkov-e2e-$E2E_NAME-screen.log"
DOS_RELEASE_TAG="${DOS_RELEASE_TAG:-0.1}"
DOS_IMAGE_URL="${DOS_IMAGE_URL:-https://github.com/ddanila/msdos/releases/download/${DOS_RELEASE_TAG}/floppy-minimal.img}"

PASS=0
FAIL=0
QEMU_PID=""

ok() {
    echo "  PASS: $1"
    PASS=$((PASS + 1))
}

fail() {
    echo "  FAIL: $1"
    FAIL=$((FAIL + 1))
}

cleanup() {
    if [[ -n "$QEMU_PID" ]]; then
        kill "$QEMU_PID" 2>/dev/null || true
        wait "$QEMU_PID" 2>/dev/null || true
    fi
    rm -f "$QMP_SOCK" "$AUTOEXEC"
}

download_base_img() {
    mkdir -p "$OUT_DIR"
    if [[ -f "$BASE_IMG" ]]; then
        return
    fi

    echo "Downloading minimal DOS floppy from $DOS_IMAGE_URL ..."
    python3 - "$DOS_IMAGE_URL" "$BASE_IMG" <<'PY'
import shutil
import sys
import urllib.request

url, dest = sys.argv[1], sys.argv[2]
with urllib.request.urlopen(url) as response, open(dest, "wb") as out:
    shutil.copyfileobj(response, out)
PY
}

trap cleanup EXIT

mkdir -p "$OUT_DIR"
download_base_img

echo "=== Volkov Commander DOS e2e ($E2E_NAME) ==="

if [[ "$E2E_NAME" == "asm" ]]; then
    echo "Building byte-identical VC.COM ..."
    KEEP_WASM_ARTIFACTS=1 "$REPO_ROOT/tools/verify_wasm_build.sh"
else
    echo "Building C recreation VC.COM ..."
    make -C "$REPO_ROOT" vc
fi

echo "Preparing boot floppy ..."
cp "$BASE_IMG" "$BOOT_IMG"
mcopy -o -i "$BOOT_IMG" "$E2E_BINARY" ::VC.COM
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
python3 "$REPO_ROOT/tests/screen_expect.py" \
    "$QMP_SOCK" "$SCREEN_LOG" \
    '1Help   2Menu   3View' ''

kill "$QEMU_PID" 2>/dev/null || true
wait "$QEMU_PID" 2>/dev/null || true
QEMU_PID=""

echo ""
echo "--- Volkov Commander checks ($E2E_TITLE) ---"

if [[ -s "$SCREEN_LOG" ]]; then
    ok "Screen log created"
else
    fail "Screen log missing or empty"
fi

if grep -q "The Volkov Commander, Version 4.05" "$SCREEN_LOG"; then
    ok "Startup banner rendered"
else
    fail "Startup banner not found"
fi

if grep -q "1Help   2Menu   3View" "$SCREEN_LOG"; then
    ok "Main function-key menu rendered"
else
    fail "Main function-key menu not found"
fi

if grep -q "vc       com" "$SCREEN_LOG"; then
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
