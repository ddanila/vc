# E2E test adoption status

Snapshot of where the kvikdos test suite + QEMU smoke adoption stand.
For the in-flight kvikdos child-spawn work referenced below see
`vendor/kvikdos/PLAN_INPLACE_SPAWN.md`.

## What's in the tree

* `vendor/kvikdos` — submodule pinned to `improvements` tip
  (currently includes in-place AH=4B AL=00 child spawn + EXE-child
  alloc fallback fix).
* `tests/test_common.h`, `tests/fixtures/`, 28 `tests/test_*.c` —
  ported from beta_kappa's `TEST_GROUPS` minus `test_screen_match`
  (recreation-only) and the recreation-specific binaries.
* `tests/test_volkov_e2e.sh`, `tests/screen_expect.py` — QEMU smoke
  test, reworked to take `<version>` and use prebuilt
  `build/<version>/VC.COM` (+ `VC.OVL` for 4.99.09). Reuses the
  floppy image `build.sh` already caches under `build/.cache/`.
* `Makefile` — top-level. Targets:
  * `make build-tests` — assembles the 28 kvikdos test binaries.
  * `make test-kvikdos-4.05` — full suite vs `build/4.05/VC.COM`.
  * `make test-kvikdos-4.99.09` — base subset vs `build/4.99.09/VC.COM`
    (editor-related tests excluded; sets `VC_OVL_PATH` so the harness
    drops `KVIKPROG.OVL`/`VC.OVL` into each test's temp dir).
  * `make test-qemu-{4.05,4.99.09}` — QEMU smoke per version.
  * `make test-{4.05,4.99.09}` / `make test` — combined.

## Per-version status

### VC 4.05

* **kvikdos:** 25/28 tests pass cleanly against `build/4.05/VC.COM`.
  Failing groups (real differences vs beta_kappa's reference VC.COM —
  our build is 64 990 bytes, theirs is 65 090):
  * `test_menu` — 36 pass, 12 fail.
  * `test_panel_adv` — 7 pass, 15 fail (was crashing kvikdos before
    the AH=4B graceful-error fix that landed in
    `kvikdos@3e21f85`; now runs to completion).
  * `test_menu_features` — 21 pass, 1 fail.
* **QEMU smoke:** not yet run end-to-end; needs the floppy image
  cached. Should work — the script is the proven beta_kappa one,
  retargeted at our prebuilt artifacts.

### VC 4.99.09

* **kvikdos:** **does not boot to the panel yet.** The 4.99.09
  resident-stub + swap-spawned-OVL architecture (see
  `vendor/kvikdos/PLAN_INPLACE_SPAWN.md`) requires kvikdos's child
  spawn to behave like real DOS:
  1. ✅ `kvikdos@4814e5f` — in-place spawn (parent memory not wiped,
     PSP[0x16] populated). Verified by
     `vendor/kvikdos/tests/inplace_spawn/run.sh` (COM child).
  2. ✅ `kvikdos@8d199d0` — AH=4B AL=00 EXE alloc falls back to the
     largest free block instead of `memsize_min_para`, so `VC.OVL`
     now receives ~640 KB. Verified by the EXE-child variant of the
     same test.
  3. ⛔ Remaining: VC.OVL still exits `AL=0xFF` at `cs:ip=0x177:0x6c`
     after running ~100 bytes of code with no INT 21h calls. Some
     other in-memory check inside the OVL is not yet satisfied
     (candidates: PSP fields beyond 0x16 / 0x02, INVARS, parent
     image bytes outside the 0x66 paragraphs we kept).
* **QEMU smoke:** not yet validated. Banner regex was loosened to
  match both versions; needs a one-time run to verify.

## Open items

### ddanila_vc

1. Decide 4.99.09 strategy until kvikdos catches up:
   * Run only the QEMU smoke for 4.99.09 (recommended), or
   * Block on the kvikdos OVL-startup investigation.
2. Run the QEMU e2e once for both versions and tighten the screen
   assertions if needed.
3. CI: extend `.github/workflows/build.yml` (or add an `e2e.yml`) to
   run `test-kvikdos-<ver>` and `test-qemu-<ver>` after the build per
   matrix entry.

### vendor/kvikdos (branch `improvements`)

1. Diagnose what VC.OVL checks at `0x177:0x9..0x6c` and what
   in-memory state we're missing. Likely tractable via `KVIKDOS_TRACE`
   + a static disassembly of the first 0x70 bytes of `VC.OVL`.
2. Once that's fixed, rerun `tests/inplace_spawn/run.sh` and the
   ddanila_vc 4.99.09 kvikdos suite.

## How to reproduce

```sh
# kvikdos suite (4.05 only today)
git -C vendor/kvikdos pull --ff-only
make build-tests
make test-kvikdos-4.05      # passes 25/28

# In-place spawn synthetic test in kvikdos itself
( cd vendor/kvikdos && bash tests/inplace_spawn/run.sh )   # passes

# 4.99.09 kvikdos (currently does not boot — see "Remaining" above)
make test-kvikdos-4.99.09
```

## Reference commits

* `vendor/kvikdos@3e21f85` — INT 21h AH=4B graceful DOS error on
  bounds-check failure (unblocked `test_panel_adv` against 4.05).
* `vendor/kvikdos@e2f7d2b` — plan + minimal in-place-spawn repro.
* `vendor/kvikdos@4814e5f` — in-place AH=4B AL=00 child spawn.
* `vendor/kvikdos@8d199d0` — AH=4B AL=00 alloc fallback to largest
  free block + EXE-child regression.
* `ddanila_vc@db5e903` — add kvikdos submodule.
* `ddanila_vc@ef65e0b` — import test suite + QEMU e2e from beta_kappa.
* `ddanila_vc@2b9cff9` — Makefile + version-aware QEMU wrapper.
* `ddanila_vc@d0ecc27` — submodule bump (AH=4B graceful error).
* `ddanila_vc@b6d0a34` — submodule bump (in-place spawn).
* `ddanila_vc@cc66b3d` — submodule bump (alloc fallback fix).
