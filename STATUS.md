# E2E test adoption status

Snapshot of where the kvikdos test suite + QEMU smoke adoption stand.
For the in-flight kvikdos child-spawn work referenced below see
`vendor/kvikdos/PLAN_INPLACE_SPAWN.md`.

## What's in the tree

* `vendor/kvikdos` — submodule pinned to `improvements` tip
  (currently includes in-place AH=4B AL=00 child spawn, alloc
  fallback, binary-safe command tail, AH=06 ZF/key fix, AH=44 AL=0F
  Set Logical Drive Map, and AH=44 AL=0D CL=0x66 Get Media ID).
* `tests/test_common.h`, `tests/fixtures/`, 28 `tests/test_*.c` —
  ported from beta_kappa's `TEST_GROUPS` minus `test_screen_match`
  (recreation-only). `setup_fixtures()` now also drops `KVIKPROG.OVL`
  + `VC.OVL` into each per-test temp dir when `VC_OVL_PATH` is set.
* `tests/test_volkov_e2e.sh`, `tests/screen_expect.py` — QEMU smoke
  test, takes `<version>` and uses prebuilt `build/<version>/VC.COM`
  (+ `VC.OVL` for 4.99.09). Reuses the floppy image `build.sh`
  caches under `build/.cache/`.
* `Makefile` — top-level. Targets:
  * `make build-tests` — assembles the 28 kvikdos test binaries.
  * `make test-kvikdos-4.05` / `make test-kvikdos-4.99.09` — the
    4.99 group excludes editor tests (`test_editor`,
    `test_deep_editors`, `test_gaps_editors`) and sets `VC_OVL_PATH`.
  * `make test-qemu-{4.05,4.99.09}` — QEMU smoke per version.
  * `make test-{4.05,4.99.09}` / `make test` — combined.
* `.github/workflows/e2e.yml` — CI matrix per VC version: build →
  build kvikdos test binaries → kvikdos suite → QEMU e2e → upload
  artifacts.

## Per-version status

### VC 4.05

* **kvikdos:** 25/28 test groups pass cleanly against
  `build/4.05/VC.COM`. Remaining failures (28 individual checks across
  3 groups) reflect real differences vs beta_kappa's reference VC.COM
  (our build is 64 990 bytes; theirs is 65 090):
  * `test_menu_features` — 21 pass, 1 fail.
    "memory info uses concrete program labels" expects
    `kviktest_find_text("VC")` in VC's Memory Info. kvikdos labels the
    program as `KVIKPROG.COM` by default (see `kvikdos.c:2757`); test
    was written when the harness happened to expose `VC` in the MCB.
    **Fix candidate:** test harness should set `dos_prog_abs` to the
    basename of `prog_path`, or the test should accept the actual
    program name.
  * `test_menu` — 36 pass, 12 fail.
  * `test_panel_adv` — 7 pass, 15 fail (used to crash kvikdos
    pre-`@3e21f85`; now runs to completion).
* **QEMU smoke:** ✅ passes 5/5.

### VC 4.99.09

* **kvikdos:** does not boot to the panel yet. Significant progress
  this session — three child-spawn fixes plus three INT 21h fixes
  (AH=06, AH=44 AL=0F, AH=44 AL=0D) all landed upstream:
  1. ✅ `kvikdos@4814e5f` — in-place spawn (parent memory not wiped,
     PSP[0x16] populated). Verified by
     `vendor/kvikdos/tests/inplace_spawn/run.sh` (COM child).
  2. ✅ `kvikdos@8d199d0` — AH=4B AL=00 EXE alloc falls back to the
     largest free block instead of `memsize_min_para`. Verified by
     the EXE-child variant of the same test.
  3. ✅ `kvikdos@553d160` — AH=4B AL=00 preserves the binary command
     tail (NUL-safe). Lets the OVL pass its sentinel signature check
     against the resident stub.
  4. ✅ `kvikdos@3bae68c` — AH=06 ZF/key-pop, AH=44 AL=0F Set Drive
     Map, AH=44 AL=0D CL=0x66 Get Media ID.

  After these the OVL completes its `Init00` / segment setup, jumps
  to its main code segment (cs:0d52), and starts driving INT 10h
  video calls — but it paints a "Can't read the disk in drive C:"
  Error dialog before any FindFirst/FindFirstFCB call, then idles in
  the dialog's keypoll loop. `kviktest_send_key(ENTER)` is now
  successfully popped (verified) but the dialog re-appears on the
  next ReadDir attempt.

  Real DOS+QEMU is fine — `make test-qemu-4.99.09` passes 5/5 — so
  the gap is something kvikdos still doesn't model that VC's
  `VCPANELS.INC:ReadDir` checks before issuing a directory read.
  `findfirst.com` (kvikdos's own regression) works against the same
  fixture dir, ruling out FindFirst itself.
* **QEMU smoke:** ✅ passes 5/5.

## Open items

### ddanila_vc

1. Diagnose what specific check inside VC 4.99.09's `ReadDir`
   triggers `Can't read disk` before any AH=4Eh call. Likely an
   in-memory check or an INT 21h call that returns the wrong code
   (e.g. `AH=44 AL=0D CL=0x60`/`0x40`/etc., or a DOS-internals read).
2. Either fix the harness so VC's Memory Info reports `VC` (not
   `KVIKPROG`), or relax `test_menu_features` accordingly.
3. Triage the 12 `test_menu` and 15 `test_panel_adv` failures —
   classify each as "tests assume reference binary layout" vs "real
   functional difference" and adjust tests or kvikdos.

### vendor/kvikdos (branch `improvements`)

* When the missing 4.99.09 startup primitive is identified, add
  the handler/stub upstream and bump the submodule.

## How to reproduce

```sh
make build-tests
make test-kvikdos-4.05      # currently 25/28 pass
make test-qemu-4.05         # 5/5 pass
make test-qemu-4.99.09      # 5/5 pass

# 4.99.09 kvikdos hangs on the disk-read dialog (see Open items)
make test-kvikdos-4.99.09

# kvikdos's own in-place-spawn regression (passes)
( cd vendor/kvikdos && bash tests/inplace_spawn/run.sh )
```

## Reference commits

* `vendor/kvikdos@3e21f85` — INT 21h AH=4B graceful DOS error on
  bounds-check failure (unblocked `test_panel_adv` against 4.05).
* `vendor/kvikdos@e2f7d2b` — plan + minimal in-place-spawn repro.
* `vendor/kvikdos@4814e5f` — in-place AH=4B AL=00 child spawn.
* `vendor/kvikdos@8d199d0` — AH=4B AL=00 alloc fallback to largest
  free block + EXE-child regression.
* `vendor/kvikdos@553d160` — AH=4B AL=00 preserves binary command
  tail (NUL-safe).
* `vendor/kvikdos@3bae68c` — AH=06 ZF/key fix + AH=44 AL=0F + AH=44
  AL=0D CL=0x66 handlers for VC 4.99.09 startup.
* `ddanila_vc@db5e903` — add kvikdos submodule.
* `ddanila_vc@ef65e0b` — import test suite + QEMU e2e from beta_kappa.
* `ddanila_vc@2b9cff9` — Makefile + version-aware QEMU wrapper.
* `ddanila_vc@b4a2c26` — wire VC_OVL_PATH for 4.99.09 + bump kvikdos.
