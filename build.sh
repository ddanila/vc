#!/usr/bin/env bash
# Build VC source trees under versions/ using TASM 4.1 / TLINK 7.1 from the
# third_party/tasm submodule, executed inside the ghcr.io/ddanila/msdos/ci
# container via dosbox.
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

if [[ ! -d "$TASM_BIN" ]]; then
    echo "third_party/tasm not initialised. Run: git submodule update --init" >&2
    exit 1
fi

# Per-version build recipes. Each recipe is a sequence of DOS commands run
# after PATH is set to TASM\BIN and the working directory is the staged
# source tree.
recipe_4_05=$(cat <<'DOS'
call MAKE.BAT VC
call MAKE.BAT VCSETUP
DOS
)

recipe_4_99_09=$(cat <<'DOS'
call MAKE.BAT
DOS
)

# Files to copy back from the staged tree as build artifacts.
artifacts_4_05='*.COM *.EXE'
artifacts_4_99_09='*.COM *.OVL'

build_version() {
    local version="$1"
    local src="$ROOT/versions/$version"
    local out="$ROOT/build/$version"
    local stage="$out/stage"

    if [[ ! -d "$src" ]]; then
        echo "no source tree at $src" >&2
        return 1
    fi

    local recipe artifacts
    case "$version" in
        4.05)     recipe="$recipe_4_05"; artifacts="$artifacts_4_05" ;;
        4.99.09)  recipe="$recipe_4_99_09"; artifacts="$artifacts_4_99_09" ;;
        *) echo "no build recipe for version '$version'" >&2; return 1 ;;
    esac

    echo "==> building $version"
    rm -rf "$out"
    mkdir -p "$stage"
    cp -R "$src"/. "$stage"/
    cp -R "$TASM_BIN" "$stage/TASM"

    # dosbox config: mount staged dir as C:, prepend TASM to PATH, run recipe.
    cat > "$stage/dosbox.conf" <<CFG
[sdl]
output=texture

[cpu]
core=auto
cycles=max

[autoexec]
mount c /work
c:
set PATH=C:\\TASM;%PATH%
$recipe
exit
CFG

    docker run --rm \
        -e SDL_VIDEODRIVER=dummy \
        -v "$stage":/work:rw \
        "$IMAGE" \
        bash -eu -c '
            export DEBIAN_FRONTEND=noninteractive
            if ! command -v dosbox >/dev/null 2>&1; then
                apt-get update -q >/dev/null
                apt-get install -y --no-install-recommends dosbox >/dev/null
            fi
            cd /work
            SDL_VIDEODRIVER=dummy dosbox -conf /work/dosbox.conf -exit \
                >/work/dosbox.log 2>&1 || {
                    echo "dosbox failed; tail of log:" >&2
                    tail -40 /work/dosbox.log >&2
                    exit 1
                }
        '

    # Collect artifacts from the staged tree into out/.
    shopt -s nullglob nocaseglob
    local found=0
    for pattern in $artifacts; do
        for f in "$stage"/$pattern; do
            cp "$f" "$out/"
            found=$((found + 1))
        done
    done
    shopt -u nullglob nocaseglob

    if [[ $found -eq 0 ]]; then
        echo "no artifacts produced for $version (see $stage/dosbox.log)" >&2
        return 1
    fi
    echo "==> $version: $found artifact(s) in $out"
}

versions=("$@")
if [[ ${#versions[@]} -eq 0 ]]; then
    versions=(4.05 4.99.09)
fi

for v in "${versions[@]}"; do
    build_version "$v"
done
