# Roadmap

UniLume is still an experimental foundation.

## Near term

- Confirm the provenance and exact license grant for inherited files without
  headers.
- Add regression vectors for punctuation, malformed sequences, macros and
  user keymaps without changing the engine algorithm.
- Add continuous integration for current Linux compilers.
- Review and approve the
  [minimal Linux adapter proposal](linux-adapter-design.md), including its
  opaque C context, C++23 wrapper, and backend-neutral edit contract.

## Later, after the foundation is verified

- Evaluate one maintained Linux input-method integration.
- Add distro packaging only when an end-user executable exists.
- Assess old XIM/GTK2 code separately; do not imply it is supported merely
  because the historical source is retained.

Large GUI, daemon, Wayland and system-hook work is intentionally outside this
repository-preparation milestone.

Implementation should follow the proposal milestones: contract tests and an
opaque context first, then a C++23 wrapper and fake backend, followed by
benchmark/fuzz work. Fcitx5 and IBus evaluation remains separate from backend
implementation and packaging.
