# Roadmap

UniLume is still an experimental foundation.

## Near term

- Confirm the provenance and exact license grant for inherited files without
  headers.
- Add regression vectors for punctuation, malformed sequences, macros and
  user keymaps without changing the engine algorithm.
- Add continuous integration for current Linux compilers.
- Define a small platform-adapter contract around the existing C API.

## Later, after the foundation is verified

- Evaluate one maintained Linux input-method integration.
- Add distro packaging only when an end-user executable exists.
- Assess old XIM/GTK2 code separately; do not imply it is supported merely
  because the historical source is retained.

Large GUI, daemon, Wayland and system-hook work is intentionally outside this
repository-preparation milestone.
