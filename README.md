# Volkov Commander — build branch

The `build` branch adds a reproducible build pipeline on top of the master
snapshot. It is intended for experimentation, not as a canonical history of
the sources.

What's added beyond master:

- `third_party/tasm` — submodule pointing at [zajo/TASM](https://github.com/zajo/TASM),
  providing TASM 4.1, TLINK 7.1, and Borland Make 4.0.
- `versions/4.99.09/` — minimal compile fixes squashed from
  [arkdevil's PR](https://github.com/ddanila/vc/pull/1) (DGROUP overflow,
  duplicated `DS:` prefix, encoding-safe rewrite of CP866 string data).
- `build.sh` — wrapper that boots MS-DOS 4.0 in QEMU
  (`floppy-minimal.img` from [ddanila/msdos release 0.1](https://github.com/ddanila/msdos/releases/tag/0.1))
  with TASM and the per-version source tree on a FAT16 data disk, and runs
  each version's `MAKE.BAT` to produce native artifacts.
- `.github/workflows/build.yml` — CI matrix that runs the same build for
  4.05 and 4.99.09 inside `ghcr.io/ddanila/msdos/ci` with KVM, uploading
  `*.COM`/`*.EXE`/`*.OVL` as workflow artifacts.
- `vendor/kvikdos`, `tests/`, top-level `Makefile`,
  `.github/workflows/e2e.yml` — end-to-end test pipeline. Two layers per
  version: a kvikdos suite that drives the built binaries through a soft
  8086 + DOS-syscall emulator, and a QEMU smoke test that boots real DOS
  with the same artifacts.

Local quick start:

```sh
git clone --recurse-submodules -b build https://github.com/ddanila/vc
cd vc
./build.sh           # builds both versions; needs Docker
./build.sh 4.99.09   # builds one
```

Output lands in `build/<version>/`. The script docker-runs the same CI image
used by GitHub Actions, so local and CI builds share an environment.

Testing the built artifacts:

```sh
make test                  # both layers, both versions
make test-kvikdos-4.05     # kvikdos suite per version
make test-kvikdos-4.99.09
make test-qemu-4.05        # QEMU smoke per version
make test-qemu-4.99.09
```

See `STATUS.md` for the current pass/fail state and the version-specific
quirks the harness branches on.
