# Volkov Commander historical sources

This repository preserves a small set of historical Volkov Commander artifacts.

## Layout

- `archives/` contains the original ZIP files, retained as provenance.
- `archives/vc005.zip` is an early binary-only/reference build.
- `versions/4.05/` contains the extracted source tree from `vc405.zip`.
- `versions/4.99.09/` contains the extracted source tree from `vc49909.zip`.

Git records the snapshot commit and tag dates from the newest timestamp inside
each source archive. Per-file modification times are preserved in the original
ZIP archives, but not in Git checkouts.
