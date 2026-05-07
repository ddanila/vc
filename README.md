# Volkov Commander historical sources

This repository preserves a small set of historical Volkov Commander artifacts.

## Historical context

In correspondence with Danila Sukharev on May 1, 2026, Vsevolod Volkov
described the origin of Volkov Commander:

> Initially, the program was conceived simply as a joke: a tiny assembler
> program that looked like NC 3.0, whose only function was to list directory
> contents. Then, in my spare time, I added individual functions: copying,
> viewing, and so on. After a while, I had something usable. Moreover, on those
> PC/XT-class computers, the program ran significantly faster and took up less
> precious RAM. I began developing it for my own use. Other users noticed the
> program, and it began to spread around the world. Back then, it didn't have
> its own name. Users came up with the name Volkov Commander.

## Layout

- `archives/` contains the original ZIP files, retained as provenance.
- `archives/vc005.zip` is an early binary-only/reference build.
- `versions/4.05/` contains the extracted source tree from `vc405.zip`.
- `versions/4.99.09/` contains the extracted source tree from `vc49909.zip`.

Git records the snapshot commit and tag dates from the newest timestamp inside
each source archive. Per-file modification times are preserved in the original
ZIP archives, but not in Git checkouts.

## Building (this branch)

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

Local quick start:

```sh
git clone --recurse-submodules -b build https://github.com/ddanila/vc
cd vc
./build.sh           # builds both versions; needs Docker
./build.sh 4.99.09   # builds one
```

Output lands in `build/<version>/`. The script docker-runs the same CI image
used by GitHub Actions, so local and CI builds share an environment.

## License

The source snapshots include the original license text in `LICENSE.TXT`:

- `versions/4.05/LICENSE.TXT`
- `versions/4.99.09/LICENSE.TXT`

`third_party/tasm/` redistributes Borland's TASM/TLINK/Make under the terms
of the upstream [zajo/TASM](https://github.com/zajo/TASM) repository and is
not covered by the VC source license.
