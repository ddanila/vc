#!/usr/bin/env bash
# Build VC source trees under versions/ using TASM 4.1 / TLINK 7.1 from the
# third_party/tasm submodule, executed under MS-DOS 4.0 in QEMU inside the
# ghcr.io/ddanila/msdos/ci container.
#
#   boot floppy : floppy-minimal.img from ddanila/msdos release 0.1
#                  + our AUTOEXEC.BAT and EXIT.COM
#   data disk   : 16 MB FAT16 image built with mtools, holding TASM, source
#                  and a per-version BUILD.BAT
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

if [[ ! -f "$CACHE/floppy-minimal.img" ]]; then
    echo "==> caching $FLOPPY_URL"
    curl -fL --retry 3 -o "$CACHE/floppy-minimal.img.tmp" "$FLOPPY_URL"
    mv "$CACHE/floppy-minimal.img.tmp" "$CACHE/floppy-minimal.img"
fi

# Tiny DOS .COM that signals QEMU's isa-debug-exit device, falling back to a
# normal DOS exit if the device isn't wired in. Assembled inside the container.
cat > "$CACHE/exit.asm" <<'ASM'
        org     100h
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
    # Only the binaries the recipes need; keeps the FAT16 image small.
    for f in TASMX.EXE TLINK.EXE TLINK.CFG; do
        cp "$TASM_BIN/$f" "$stage/tasm/"
    done
    cp "$CACHE/floppy-minimal.img" "$stage/boot.img"

    # AUTOEXEC.BAT lives on the boot floppy. Switches to C:, runs the build,
    # then shuts down QEMU via EXIT.COM.
    cat > "$stage/AUTOEXEC.BAT" <<'DOS'
@ECHO OFF
PATH=C:\TASM
C:
CD \SRC
CALL C:\BUILD.BAT > C:\BUILD.LOG
A:\EXIT.COM
DOS

    {
        echo '@ECHO OFF'
        echo "$recipe"
    } > "$stage/BUILD.BAT"

    docker run --rm \
        -v "$stage":/stage:rw \
        -v "$CACHE":/cache:ro \
        -e MTOOLS_SKIP_CHECK=1 \
        "$IMAGE" \
        bash -eux -c '
            cd /stage

            nasm -f bin -o EXIT.COM /cache/exit.asm

            # 16 MB raw HDD with one primary FAT16 partition starting at sector 63.
            qemu-img create -f raw work.img 16M >/dev/null
            mpartition -I -i work.img
            mpartition -c -t 32 -h 16 -s 63 -b 63 -l 32193 -i work.img 1
            mformat -F -i work.img@@32256 ::

            mmd  -i work.img@@32256 ::SRC ::TASM
            mcopy -i work.img@@32256 source/* ::SRC/
            mcopy -i work.img@@32256 tasm/*   ::TASM/
            mcopy -i work.img@@32256 BUILD.BAT ::

            # Overwrite AUTOEXEC.BAT and add EXIT.COM on the boot floppy.
            mcopy -o -i boot.img AUTOEXEC.BAT ::
            mcopy -o -i boot.img EXIT.COM     ::

            # Run the build. isa-debug-exit makes QEMU exit cleanly when
            # EXIT.COM writes to port 0xF4. The exit status will be (0<<1)|1=1,
            # so a non-zero exit here is the success signal — we ignore it
            # and judge success by whether artifacts came out.
            timeout 180 qemu-system-i386 \
                -nographic -display none -serial null \
                -m 16 \
                -drive if=floppy,format=raw,file=boot.img \
                -drive if=ide,format=raw,file=work.img \
                -boot a \
                -no-reboot \
                -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
                || true

            mcopy -i work.img@@32256 ::BUILD.LOG  /stage/build.log 2>/dev/null || true
            mkdir -p /stage/artifacts
            mcopy -s -i work.img@@32256 ::SRC /stage/artifacts/ 2>/dev/null || true
        '

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
