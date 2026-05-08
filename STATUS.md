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

* **kvikdos:** boots through the resident → OVL handshake. The 4.99.09
  resident-stub + swap-spawned-OVL architecture (see
  `vendor/kvikdos/PLAN_INPLACE_SPAWN.md`) needed three fixes — all
  landed:
  1. ✅ `kvikdos@4814e5f` — in-place spawn (parent memory not wiped,
     PSP[0x16] populated). Verified by
     `vendor/kvikdos/tests/inplace_spawn/run.sh` (COM child).
  2. ✅ `kvikdos@8d199d0` — AH=4B AL=00 EXE alloc falls back to the
     largest free block instead of `memsize_min_para`, so `VC.OVL`
     now receives ~640 KB. Verified by the EXE-child variant of the
     same test.
  3. ✅ `kvikdos@553d160` — AH=4B AL=00 preserves the binary command
     tail across the spawn (was previously `strcpy()`-truncated at
     the literal `\x00` inside Volkov's `OvlPrm` payload, which made
     the OVL's CMPSB signature check fail). Found by reading
     `versions/4.99.09/VCOVL.ASM:151-184` directly instead of
     disassembling.

  Status after the three fixes: the OVL completes its signature
  check, performs `Init00` setup, switches to its main code segment,
  and starts driving INT 10h video calls + INT 21h DOS calls. The
  `test_basic` watchdog still trips because the harness waits for the
  `C:\>` prompt at row 23 col 0 — VC 4.99.09's startup banner
  ("Can not read the setup file: C:\VC.INI / Use Shift-F9 to create
  a new setup file") may need a keypress to dismiss, or the prompt
  lands at a different row. Diagnose by dumping the screen at
  timeout, then either send a keypress in the harness or relax the
  prompt-detection rule for 4.99.09.
* **QEMU smoke:** not yet validated. Banner regex was loosened to
  match both versions; needs a one-time run to verify.

## Open items

### ddanila_vc

1. Diagnose VC 4.99.09's startup banner / prompt position so
   `test_basic` can see the panel render. Likely a one-line tweak to
   either feed a keypress or accept an alternative "ready" indicator.
2. Run the kvikdos suite against 4.99.09 once the prompt is detected;
   compare pass rate to 4.05's 25/28.
3. Run the QEMU e2e once for both versions and tighten the screen
   assertions if needed.
4. CI: extend `.github/workflows/build.yml` (or add an `e2e.yml`) to
   run `test-kvikdos-<ver>` and `test-qemu-<ver>` after the build per
   matrix entry.

### vendor/kvikdos (branch `improvements`)

* Three child-spawn fixes have landed and the synthetic
  `tests/inplace_spawn/run.sh` regression covers both variants. No
  open kvikdos work for VC 4.99.09 right now; reopen if the wider
  test suite uncovers something new.

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
* `vendor/kvikdos@553d160` — AH=4B AL=00 preserves binary command
  tail (NUL-safe). Lets VC 4.99.09's OVL pass its signature check.
* `ddanila_vc@db5e903` — add kvikdos submodule.
* `ddanila_vc@ef65e0b` — import test suite + QEMU e2e from beta_kappa.
* `ddanila_vc@2b9cff9` — Makefile + version-aware QEMU wrapper.
* `ddanila_vc@d0ecc27` — submodule bump (AH=4B graceful error).
* `ddanila_vc@b6d0a34` — submodule bump (in-place spawn).
* `ddanila_vc@cc66b3d` — submodule bump (alloc fallback fix).
