# JWasm (vendored binaries)

Prebuilt [JWasm](https://github.com/Baron-von-Riedesel/JWasm) assembler
binaries, used for the native (no DOS/QEMU) build of `versions/4.99.09/VC.ASM`
— see the `jwasm` step in `build.sh`. TASM remains the canonical toolchain;
the JWasm build is additive.

JWasm is licensed under the Sybase Open Watcom Public License v1.0 (see the
headers in the upstream sources).

## Provenance

Both binaries are built from upstream commit
`a7c6e70a63cf364b05e44decafc338b4915a8bb0` (2026-06-10). A tagged release
cannot be used yet: assembling `VCOVL.ASM` needs the constant-division fix
`89fc1ee4f` (2026-05-25), which is newer than the latest release (v2.21pre1).

### bin/jwasm-linux-x86_64

Built on GitHub Actions `ubuntu-latest`, statically linked (runs in any
container image):

    git clone https://github.com/Baron-von-Riedesel/JWasm.git
    cd JWasm && git checkout a7c6e70a63cf364b05e44decafc338b4915a8bb0
    make -f GccUnix.mak
    gcc build/GccUnixR/*.o -s -static -o jwasm-linux-x86_64

### bin/jwasm-macos-arm64

Built on macOS (Apple clang). macOS has no `malloc.h` and Apple's linker has
no `-Map`, hence the stub include and the manual link:

    git clone https://github.com/Baron-von-Riedesel/JWasm.git
    cd JWasm && git checkout a7c6e70a63cf364b05e44decafc338b4915a8bb0
    mkdir compat && echo '#include <stdlib.h>' > compat/malloc.h
    make -f GccUnix.mak "extra_c_flags=-DNDEBUG -O2 -Icompat" || true  # link step fails
    cc build/GccUnixR/*.o -o jwasm-macos-arm64 && strip jwasm-macos-arm64

## Byte-identical to TASM

`jwasm -Zg -Zne -DOFFICIAL -bin -FoVC.COM VC.ASM` produces a VC.COM that is byte-for-byte
identical to the TASM build.

Previously the two differed in 9 instructions (27 bytes): for `CMP AX,imm` /
`ADD AX,imm` with a small immediate, TASM emits the accumulator form
(`3D iw` / `05 iw`) while JWasm picks the equal-length sign-extended form
(`83 /7 ib` / `83 /0 ib`). Qualifying those immediates with `WORD PTR` in
`VC.ASM` forces JWasm onto the accumulator form and is a no-op for TASM (which
already used it), so both toolchains now agree. See PR #21 for the original
list of the 9 sites.
