# Testing

The `build` branch ships two test layers per VC version:

* **kvikdos** — drives `build/<version>/VC.COM` (+ `VC.OVL` on 4.99.09)
  through a soft 8086 + DOS-syscall emulator. Each test group is a
  small C binary that reads/writes the emulated screen and key buffer.
* **QEMU smoke** — boots real DOS in QEMU with the same artifacts on
  a FAT16 floppy and checks a few panel screens.

Both run from the same top-level `Makefile`.

## Layout

* `vendor/kvikdos` — submodule (the `improvements` branch tip).
* `tests/test_common.h`, `tests/fixtures/`, `tests/test_*.c` — the
  kvikdos suite. `setup_fixtures()` lays out a temp dir per test;
  when `VC_OVL_PATH` is set it also drops `KVIKPROG.OVL`, `VC.OVL`,
  and the captured 4.99 INI next to the program. `test_is_vc_499()`
  is the version split that test code branches on.
* `tests/test_volkov_e2e.sh`, `tests/screen_expect.py` — the QEMU
  smoke. Takes a `<version>` argument and reuses the floppy image
  cached under `build/.cache/`.
* `.github/workflows/e2e.yml` — CI matrix that builds, runs both
  layers, and uploads artifacts per version.

## Running

```sh
make build-tests
make test-kvikdos-4.05
make test-kvikdos-4.99.09
make test-qemu-4.05
make test-qemu-4.99.09
make test                   # all of the above
```

## Per-version status

* **VC 4.05** — kvikdos suite ✅, QEMU smoke ✅.
* **VC 4.99.09** — kvikdos base groups ✅, QEMU smoke ✅. Editor
  groups (`test_editor`, `test_deep_editors`, `test_gaps_editors`)
  are excluded — 4.99 has no built-in editor; F4 invokes whatever
  `VC.EXT` configures.

## 4.99.09 quirks the harness branches on

These are the recurring places `test_is_vc_499()` (or its Makefile
equivalent) triggers a different code path. Useful context when
adding new tests:

* **Numpad scancodes leak to cmdline.** Gray-* / Numpad-+ / Numpad--
  reach the command line as literal characters instead of "invert
  selection" / "select by pattern" / "deselect by pattern". Tests
  use Insert toggling for deselect and skip the pattern-based group.
* **Dropdown layout changes.** The Left/Right "view mode" dropdown
  inserts `Description` and `Quick view` above `Tree`, so position-
  based DOWN×N walks land on the wrong item. Tests use letter
  hotkeys (`type_string("t")` for Tree, `"b"` for Brief).
* **F3/F4 open a confirmation dialog.** "View the file: …" /
  "Edit the file: …" appears before the viewer/editor opens. Tests
  Enter-confirm if the dialog is detected.
* **Help viewer hyperlinks are unstable.** F7 search, Tab+Enter on
  links, and ESC-after-link reliably tear VC down under the
  overlay. The deep help-traversal subtests skip under 4.99.
* **Ctrl+I (= Tab) is unbound.** 4.05 binds it to "insert selected
  filenames"; 4.99 treats it as a plain Tab and switches the active
  panel. The corresponding subtest skips under 4.99.
* **Single ESC closes help.** 4.05 needs two; under 4.99 the second
  ESC at the panels exits VC. Tests gate the second ESC.

If 4.99 ever gains a binding that lets one of these subtests run as
written, the gate is a local `if (test_is_vc_499())` and is
straightforward to flip.
