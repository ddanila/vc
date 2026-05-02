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

## License

The source snapshots include the original license text in `LICENSE.TXT`:

- `versions/4.05/LICENSE.TXT`
- `versions/4.99.09/LICENSE.TXT`
