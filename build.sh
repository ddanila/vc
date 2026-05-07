#!/usr/bin/env bash
# Build VC source trees under versions/ using TASM 4.1 / TLINK 7.1 from the
# third_party/tasm submodule, executed under MS-DOS 4.0 in QEMU.
#
#   boot floppy : floppy-minimal.img from ddanila/msdos release 0.1
#                  + AUTOEXEC.BAT, EXIT.COM, MARK.COM
#   data disk   : 32 MB FAT16 image as IDE drive C:, holds TASM, source and
#                  receives the build artifacts
#
# The 32 MB / FAT16 / FAT-cluster=8 / hidden-sectors=63 layout below was
# reverse-engineered from a known-good image produced for ddanila/aml2. The
# critical detail is the OEM ID at offset 0x03 of the partition's boot
# sector: it MUST be a recognised "MSDOS<n>.0" string ("MSDOS4.0" matching
# our booting DOS or "MSDOS5.0" both work) or DOS 4.0 mounts the partition
# but reads from misaligned sectors — writes appear to succeed but show up
# at wrong clusters from mtools' perspective. mformat's default
# "MTOO<version>" OEM ID trips this; we overwrite it with dd after mformat.
#
# The script is two-mode:
#   - Outside the build container (default): docker-runs the CI image and
#     re-execs itself with VC_BUILD_IN_CONTAINER=1.
#   - Inside the container (CI): runs the build steps directly.
#
# Usage:
#   ./build.sh              # build all known versions
#   ./build.sh 4.05         # build a single version
#
# Output: build/<version>/ contains the produced .COM/.EXE/.OVL artifacts.

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

# MARK.COM writes the first character of its tail (DOS PSP[80h]) to QEMU's
# debugcon (port 0xE9) and to COM1 after a 115200 8N1 UART init, giving us
# two independent traces of how far AUTOEXEC.BAT got. EXIT.COM emits 'D' the
# same way and then triggers isa-debug-exit (port 0xF4) for clean shutdown.
cat > "$CACHE/mark.asm" <<'ASM'
        org     100h
        mov     si, 81h
        lodsb
        cmp     al, 20h
        jne     .have
        lodsb
.have:  mov     bl, al
        call    write_byte
        mov     ax, 4C00h
        int     21h

write_byte:
        mov     al, bl
        mov     dx, 0E9h
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
        ; flush DOS disk buffers before yanking the cord
        mov     ah, 0Dh
        int     21h
        mov     dx, 0F4h
        xor     al, al
        out     dx, al
        mov     ax, 4C00h
        int     21h
ASM

# Per-version build recipe: list of DOS commands run from C:\SRC after
# PATH=C:\TASM. Inlined into BUILD.BAT with line-by-line redirection of
# stdout into C:\BUILD.LOG.
recipe_4_05='TASMX /z/m9/kh10000 VC;
TLINK /t/x VC;
TASMX /z/m9/kh10000 VCSETUP;
TLINK /t/x VCSETUP;
DEL *.OBJ'

recipe_4_99_09='TASMX /z/m9/kh10000 /dOFFICIAL=1 vc.asm
TLINK /t vc, VC.COM
TASMX /z/m9/kh10000 /dOFFICIAL=1 vcovl.asm
TLINK vcovl, VC.OVL
DEL *.OBJ
DEL *.MAP'

# TASM essentials needed by TASMX 4.1: TASMX itself, TLINK, TLINK config,
# the 16-bit DPMI overlay, and Borland's Run-Time Manager. Without RTM.EXE
# and DPMI16BI.OVL TASMX prints "Stub error (200x)" on its own.
TASM_FILES=(TASMX.EXE TLINK.EXE TLINK.CFG DPMI16BI.OVL RTM.EXE)

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
    mkdir -p "$stage"

    cp "$CACHE/floppy-minimal.img" "$stage/boot.img"

    # 32 MB raw HDD, single FAT16 partition starting at sector 63. Geometry
    # 64 cyls / 16 heads / 63 secs == 64512 sectors == 32 MB exact.
    truncate -s 33030144 "$stage/work.img"

    cat > "$stage/AUTOEXEC.BAT" <<'DOS'
@ECHO OFF
A:\MARK.COM A
C:
PATH=C:\TASM
CD \SRC
A:\MARK.COM S
CALL C:\BUILD.BAT
A:\MARK.COM B
A:\EXIT.COM
DOS

    {
        echo '@ECHO OFF'
        printf 'A:\\MARK.COM 1\n'
        printf 'ECHO ----- build %s start ----- > C:\\BUILD.LOG\n' "$version"
        printf 'A:\\MARK.COM 2\n'
        local step=3
        while IFS= read -r cmd; do
            [[ -z "$cmd" ]] && continue
            printf '%s >> C:\\BUILD.LOG\n' "$cmd"
            printf 'A:\\MARK.COM %d\n' "$step"
            step=$((step + 1))
        done <<< "$recipe"
        printf 'DIR /B >> C:\\BUILD.LOG\n'
        printf 'A:\\MARK.COM 9\n'
    } > "$stage/BUILD.BAT"

    export MTOOLS_SKIP_CHECK=1
    cd "$stage"

    nasm -f bin -o EXIT.COM "$CACHE/exit.asm"
    nasm -f bin -o MARK.COM "$CACHE/mark.asm"

    cat > "$stage/mtoolsrc" <<EOF
drive a: file="$stage/boot.img"
drive c: file="$stage/work.img" partition=1
EOF
    export MTOOLSRC="$stage/mtoolsrc"

    mpartition -I c:
    mpartition -c -t 64 -h 16 -s 63 -b 63 -l 64449 -T 6 c:
    mformat -H 63 -c 8 c:

    # Replace mtools' "MTOO<ver>" OEM ID at offset 0x03 of the partition's
    # boot sector (file offset 32256 + 3) with "MSDOS4.0". DOS 4.0 mis-mounts
    # the FAT otherwise — see the file header comment. Confirmed working with
    # both "MSDOS4.0" and "MSDOS5.0"; we use the former since that matches
    # the booting DOS version.
    printf 'MSDOS4.0' | dd of="$stage/work.img" bs=1 \
        seek=$((32256 + 3)) count=8 conv=notrunc status=none

    mmd c:SRC c:TASM
    mcopy "$src"/* c:SRC/
    for f in "${TASM_FILES[@]}"; do
        mcopy "$TASM_BIN/$f" c:TASM/
    done
    mcopy -t BUILD.BAT c:

    mcopy -o -t AUTOEXEC.BAT a:
    mcopy -o EXIT.COM        a:
    mcopy -o MARK.COM        a:

    timeout 240 qemu-system-i386 \
        -display none \
        -serial "file:$stage/serial.log" \
        -debugcon "file:$stage/debugcon.log" \
        -machine pc,accel=kvm:tcg \
        -m 16 \
        -drive if=floppy,index=0,format=raw,file=boot.img,cache=writethrough \
        -drive if=ide,format=raw,file=work.img,cache=writethrough \
        -boot a \
        -no-reboot \
        -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
        || true

    echo "    debugcon trace: $(tr -d '\0' < "$stage/debugcon.log" 2>/dev/null || true)"

    mcopy c:BUILD.LOG "$stage/build.log" 2>/dev/null || true
    if [[ -f "$stage/build.log" ]]; then
        echo "--- BUILD.LOG ---" >&2
        cat "$stage/build.log" >&2
        echo "--- end BUILD.LOG ---" >&2
    fi

    cd "$ROOT"

    mkdir -p "$stage/artifacts"
    mcopy -s c:SRC "$stage/artifacts/" 2>/dev/null || true

    local copied=0
    shopt -s nullglob nocaseglob
    if [[ -d "$stage/artifacts/SRC" ]]; then
        for f in "$stage/artifacts/SRC"/*.com "$stage/artifacts/SRC"/*.exe "$stage/artifacts/SRC"/*.ovl; do
            [[ -f "$f" ]] || continue
            local base
            base="$(basename "$f")"
            if [[ ! -e "$src/$base" ]]; then
                cp "$f" "$out/"
                copied=$((copied + 1))
            fi
        done
    fi
    shopt -u nullglob nocaseglob

    if [[ "$copied" -eq 0 ]]; then
        echo "no artifacts produced for $version" >&2
        return 1
    fi
    echo "==> $version: $copied artifact(s) in $out"
}

versions=("$@")
if [[ ${#versions[@]} -eq 0 ]]; then
    versions=(4.05 4.99.09)
fi

for v in "${versions[@]}"; do
    build_version "$v"
done
