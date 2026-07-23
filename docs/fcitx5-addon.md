<!-- SPDX-License-Identifier: GPL-2.0-or-later -->

# Experimental Fcitx5 addon

UniLume now has an optional Fcitx5 input-method addon. It is an experimental
MVP, not a production-ready package.

## Build

Install CMake, pkg-config, a C++23 compiler, and Fcitx5 core development files.
On Debian-family systems the Fcitx dependency is:

```sh
sudo apt-get install libfcitx5core-dev pkg-config
```

Build and install into a disposable prefix:

```sh
PREFIX="$(mktemp -d)"

cmake -S . -B build/fcitx5 \
  -DCMAKE_BUILD_TYPE=Release \
  -DUNILUME_BUILD_FCITX5_ADDON=ON \
  -DCMAKE_INSTALL_PREFIX="$PREFIX"
cmake --build build/fcitx5 --parallel 2
cmake --install build/fcitx5
```

The prefix contains:

```text
lib/fcitx5/unilume.so
share/fcitx5/addon/unilume.conf
share/fcitx5/inputmethod/unilume.conf
```

With the option left `OFF`, normal core builds do not require Fcitx5. With the
option `ON`, configure fails clearly if the development package is missing.

For a user installation, choose a prefix recognized by that Fcitx5 package,
reload/restart Fcitx5, and add UniLume through `fcitx5-configtool`. Distribution
paths differ, so packaging must set the final prefix explicitly.

## Direct-commit behavior

Each Fcitx input context owns an independent engine/controller state. Ordinary
text is committed immediately. When Telex changes text already committed,
the backend requests the minimum surrounding-text deletion and commits the
replacement in the same synchronous controller transaction. Cursor movement,
focus changes, reset events, and unhandled Backspace clear composition state.

The default path has no client preedit and therefore no underline. Fcitx
preedit is not implemented as a fallback in this MVP.

## Safety fallback

Replacement requires the Fcitx `SurroundingText` capability plus either text
committed by the current state or a valid, unselected surrounding-text cursor
with enough characters. If the condition is not trustworthy, UniLume resets
composition and commits the triggering raw key instead of guessing a deletion.

This policy prefers a visible uncomposed key over duplicate or missing text.
It does not synthesize Backspace, sleep, retry indefinitely, use a socket
daemon, or provide a uinput fallback.

Fcitx delete/commit methods are synchronous requests on its event thread and
do not acknowledge application-side mutation. Firefox/Electron behavior still
requires manual verification against real frontends.

## Verification status

Automated:

- addon compilation and dynamic linking with Fcitx5 5.1.12 headers/runtime;
- install into a temporary prefix and metadata/factory-symbol checks;
- controller tests for immediate, delayed, stale, duplicate, reordered,
  dropped, failure, reset, burst, and sanitizer profiles.

Not yet manually verified in this milestone:

- terminal application;
- VSCode;
- Firefox;
- Chromium/Electron;
- representative GTK and Qt text fields.

No GUI configuration, VNI/VIQR selection, macros, legacy charset output,
uinput, distro package, or system-wide Wayland protocol integration is
provided yet. Telex/UTF-8 is the only exposed addon mode.
