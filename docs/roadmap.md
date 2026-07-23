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

## Experimental integration milestone

- Validate the Fcitx5 direct-commit MVP in terminal, Firefox, Electron, GTK
  and Qt applications.
- Decide whether a real preedit fallback is needed for clients without
  trustworthy surrounding text.
- Add allocation-per-key instrumentation only when it can cover the C and C++
  paths without distorting the hot path.
- Stabilize integration metrics and failure policy before treating the addon
  as an end-user input method.

## Later

- Add distro packaging only when an end-user executable exists.
- Evaluate other maintained input-method integrations without coupling the
  core contract to Fcitx5.
- Assess old XIM/GTK2 code separately; do not imply it is supported merely
  because the historical source is retained.

Large GUI, daemon, Wayland and system-hook work is intentionally outside this
integration milestone.

The opaque context, C++23 controller, deterministic backend and optional
Fcitx5 addon now exist. Remaining work must preserve their backend-neutral
contract and keep desktop-specific policy out of the inherited engine.
