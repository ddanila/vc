#!/usr/bin/env bash
# Build VC source trees under versions/ using TASM 4.1 / TLINK 7.1 from the
# third_party/tasm submodule, executed under MS-DOS 4.0 in QEMU.
#
#   boot floppy : floppy-minimal.img from ddanila/msdos release 0.1
#                  + our AUTOEXEC.BAT and EXIT.COM
#   data disk   : 16 MB FAT16 image built with mtools, holding TASM, source
#                  and a per-version BUILD.BAT
#
# The script is two-mode:
#   - Outside the build container (default): docker-runs ghcr.io/ddanila/msdos/ci
#     and re-execs itself inside with VC_BUILD_IN_CONTAINER=1.
#   - Inside the container (CI workflows that already use container:): runs
#     the build steps directly. Set VC_BUILD_IN_CONTAINER=1 to force this.
#
# Usage:
#   ./build.sh              # build all known versions
#   ./build.sh 4.99.09      # build a single version
#
# Output: build/<version>/ contains the produced .COM/.OVL artifacts.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
IMAGE="${VC_BUILD_IMAGE:-ghcr.io/ddanila/msdos/ci:latest}"
TASM_BIN="$ROOT/third_party/tasm/BIN"
CACHE="$ROOT/build/.cache"
FLOPPY_URL="https://github.com/ddanila/msdos/releases/download/0.1/floppy-minimal.img"

[[ -d "$TASM_BIN" ]] || { echo "third_party/tasm not initialised. Run: git submodule update --init" >&2; exit 1; }
mkdir -p "$CACHE"

download() {
    local url="$1" out="$2"
    if command -v curl >/dev/null 2>&1; then
        curl -fL --retry 3 -o "$out.tmp" "$url"
    elif command -v wget >/dev/null 2>&1; then
        wget -q -O "$out.tmp" "$url"
    else
        python3 -c "import sys,urllib.request as u; u.urlretrieve(*sys.argv[1:])" "$url" "$out.tmp"
    fi
    mv "$out.tmp" "$out"
}

if [[ ! -f "$CACHE/floppy-minimal.img" ]]; then
    echo "==> caching $FLOPPY_URL"
    download "$FLOPPY_URL" "$CACHE/floppy-minimal.img"
fi

# When invoked outside the build container, wrap ourselves in docker.
if [[ "${VC_BUILD_IN_CONTAINER:-0}" != "1" ]]; then
    kvm_opts=()
    [[ -e /dev/kvm ]] && kvm_opts=(--device /dev/kvm)
    exec docker run --rm \
        -v "$ROOT":/repo:rw \
        -w /repo \
        -e VC_BUILD_IN_CONTAINER=1 \
        "${kvm_opts[@]}" \
        "$IMAGE" \
        ./build.sh "$@"
fi

# === From here we are running inside ghcr.io/ddanila/msdos/ci ===

# MARK.COM writes the first character of its tail (DOS PSP[80h]) to:
#   - QEMU debugcon (port 0xE9) — captured via -debugcon
#   - COM1 (port 0x3F8) after a quick UART init at 115200 8N1 — captured via -serial
# EXIT.COM emits 'D' the same way and then triggers isa-debug-exit (0xF4) so
# QEMU exits cleanly. Two channels gives us redundancy if one is misrouted.
cat > "$CACHE/mark.asm" <<'ASM'
        org     100h
        mov     si, 81h         ; PSP cmdline (preceded by length at 80h)
        lodsb
        cmp     al, 20h         ; skip leading space
        jne     .have
        lodsb
.have:  mov     bl, al
        call    write_byte
        mov     ax, 4C00h
        int     21h

write_byte:
        mov     al, bl
        mov     dx, 0E9h
        out     dx, al          ; debugcon
        ; UART init: divisor 1 = 115200 bps
        mov     dx, 3FBh
        mov     al, 80h
        out     dx, al          ; LCR DLAB on
        mov     dx, 3F8h
        mov     al, 1
        out     dx, al          ; DLL
        mov     dx, 3F9h
        xor     al, al
        out     dx, al          ; DLM
        mov     dx, 3FBh
        mov     al, 03h
        out     dx, al          ; LCR 8N1, DLAB off
        mov     dx, 3F8h
        mov     al, bl
        out     dx, al          ; THR
        ret
ASM
cat > "$CACHE/exit.asm" <<'ASM'
        org     100h
        mov     bl, 'D'
        mov     dx, 0E9h
        mov     al, bl
        out     dx, al
        mov     dx, 3FBh
        mov     al, 80h
        out     dx, al
        mov     dx, 3F8h
        mov     al, 1
        out     dx, al
        mov     dx, 3F9h
        xor     al, al
        out     dx, al
        mov     dx, 3FBh
        mov     al, 03h
        out     dx, al
        mov     dx, 3F8h
        mov     al, bl
        out     dx, al
        mov     dx, 0F4h
        xor     al, al
        out     dx, al
        mov     ax, 4C00h
        int     21h
ASM

# Per-version build recipe: DOS commands run from C:\SRC after PATH=C:\TASM.
recipe_4_05='MAKE.BAT VC
MAKE.BAT VCSETUP'

recipe_4_99_09='MAKE.BAT'

build_version() {
    local version="$1"
    local src="$ROOT/versions/$version"
    local out="$ROOT/build/$version"
    local stage="$out/stage"

    [[ -d "$src" ]] || { echo "no source tree at $src" >&2; return 1; }

    local recipe
    case "$version" in
        4.05)    recipe="$recipe_4_05" ;;
        4.99.09) recipe="$recipe_4_99_09" ;;
        *) echo "no build recipe for version '$version'" >&2; return 1 ;;
    esac

    echo "==> building $version"
    rm -rf "$out"
    mkdir -p "$stage/source" "$stage/tasm"
    cp -R "$src"/. "$stage/source"/
    for f in TASMX.EXE TLINK.EXE TLINK.CFG; do
        cp "$TASM_BIN/$f" "$stage/tasm/"
    done
    cp "$CACHE/floppy-minimal.img" "$stage/boot.img"

    # ISOLATION TEST: just probe whether DOS boots and runs AUTOEXEC at all.
    # Once 'A' shows up via debugcon or serial, restore the real recipe.
    cat > "$stage/AUTOEXEC.BAT" <<'DOS'
@ECHO OFF
A:\MARK.COM A
A:\EXIT.COM
DOS

    {
        echo '@ECHO OFF'
        echo "$recipe"
    } > "$stage/BUILD.BAT"

    export MTOOLS_SKIP_CHECK=1
    cd "$stage"

    nasm -f bin -o EXIT.COM "$CACHE/exit.asm"
    nasm -f bin -o MARK.COM "$CACHE/mark.asm"

    # 16 MB raw HDD with one primary FAT16 partition starting at sector 63.
    truncate -s 16M work.img

    cat > "$stage/mtoolsrc" <<EOF
drive a: file="$stage/boot.img"
drive c: file="$stage/work.img" partition=1
EOF
    export MTOOLSRC="$stage/mtoolsrc"

    mpartition -I c:
    mpartition -c -t 32 -h 16 -s 63 -b 63 -l 32193 c:
    mformat -F c:

    mmd c:SRC c:TASM
    mcopy source/* c:SRC/
    mcopy tasm/*   c:TASM/
    mcopy BUILD.BAT c:

    # Overwrite AUTOEXEC.BAT and add EXIT.COM / MARK.COM on the boot floppy.
    mcopy -o AUTOEXEC.BAT a:
    mcopy -o EXIT.COM     a:
    mcopy -o MARK.COM     a:

    echo "--- floppy contents ---" >&2
    mdir -a a: >&2 || true
    echo "--- HDD contents ---" >&2
    mdir -a c: >&2 || true
    mdir -a c:SRC >&2 || true
    mdir -a c:TASM >&2 || true
    echo "--- AUTOEXEC.BAT on floppy ---" >&2
    mtype a:AUTOEXEC.BAT >&2 || true
    echo "--- end pre-boot dump ---" >&2

    # isa-debug-exit makes QEMU exit cleanly when EXIT.COM writes to port 0xF4.
    # Exit status will be (0<<1)|1 = 1, so a non-zero exit here is the success
    # signal; we ignore it and judge success by whether artifacts came out.
    timeout 60 qemu-system-i386 \
        -display none \
        -serial "file:$stage/serial.log" \
        -debugcon "file:$stage/debugcon.log" \
        -machine pc,accel=kvm:tcg \
        -m 4 \
        -drive if=floppy,index=0,format=raw,file=boot.img,cache=writethrough \
        -boot a \
        -no-reboot \
        -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
        || true

    echo "    debugcon trace: $(tr -d '\0' < "$stage/debugcon.log" 2>/dev/null || true)"
    echo "    serial trace:   $(tr -d '\0\r' < "$stage/serial.log"   2>/dev/null || true)"

    mcopy c:BUILD.LOG "$stage/build.log" 2>/dev/null || true
    mkdir -p "$stage/artifacts"
    mcopy -s c:SRC "$stage/artifacts/" 2>/dev/null || true

    cd "$ROOT"

    # Promote freshly-built artifacts (anything that did not exist in the
    # original source tree) into out/.
    if [[ -d "$stage/artifacts/SRC" ]]; then
        shopt -s nullglob nocaseglob
        for f in "$stage/artifacts/SRC"/*.com "$stage/artifacts/SRC"/*.exe "$stage/artifacts/SRC"/*.ovl; do
            local base
            base="$(basename "$f")"
            if [[ ! -e "$src/$base" ]]; then
                cp "$f" "$out/"
            fi
        done
        shopt -u nullglob nocaseglob
    fi

    local count
    count=$(find "$out" -maxdepth 1 -type f | wc -l | tr -d ' ')
    if [[ "$count" -eq 0 ]]; then
        echo "no artifacts produced for $version" >&2
        if [[ -f "$stage/build.log" ]]; then
            echo "--- DOS build log ---" >&2
            cat "$stage/build.log" >&2
        fi
        return 1
    fi
    echo "==> $version: $count artifact(s) in $out"
}

versions=("$@")
if [[ ${#versions[@]} -eq 0 ]]; then
    versions=(4.05 4.99.09)
fi

for v in "${versions[@]}"; do
    build_version "$v"
done
