<!-- SPDX-License-Identifier: GPL-2.0-or-later -->

# Browser input policy

This document describes how UniLume selects the input path for browser and
Electron frontends and why some frontends show client-preedit underlines while
others use direct zero-preedit.

## Capability requirement

UniLume's direct zero-preedit path requires the Fcitx
`CapabilityFlag::SurroundingText` (bit 6) to be set by the frontend. This flag
signals that the application can provide the text around the cursor, letting the
input method delete and replace previously committed characters safely.

The following Fcitx 5.1.12 capability flags are relevant:

| Flag | Value | Purpose |
| --- | --- | --- |
| `SurroundingText` | 1 << 6 | Surrounding text available for replacement |
| `Preedit` | 1 << 1 | Frontend supports client-side preedit |

## Input path selection

The `InputModePolicy` state machine chooses one of three paths:

```
                    ┌──────────┐
     first key ────▶│  Unknown │◀── reset/focus
                    └────┬─────┘
                         │
              ┌──────────┴──────────┐
              │                     │
      SurroundingText         no SurroundingText
         available              (browser/Electron)
              │                     │
              ▼                     ▼
        ┌──────────┐         ┌──────────┐
        │  Direct  │         │ Preedit  │
        │ zero-    │         │ client   │
        │ preedit  │         │ fallback │
        └──────────┘         └──────────┘
              │                     │
              │   cap. lost         │
              ├────────────────────▶│
              │                     │
              │    composition      │
              │    reset/end        │
              │                     │
              └────► Unknown ◄──────┘
```

**Rules:**

1. Path is selected on the first key event of a composition based on the
   current `SurroundingText` capability.
2. During an active composition the path is stable:
   - `preedit` never promotes to `direct` (prevents overlapping preedit
     update from Firefox)
   - `direct` demotes to `preedit` only if `SurroundingText` is lost
3. Between compositions (after commit, reset, focus change, or navigation):
   - Both paths reset to `unknown`
   - The next composition re-evaluates the capability snapshot
4. A full `reset()` (focus change) always returns to `unknown`.

## Browser capability observations

From the real-application validation
(`docs/real-application-validation.md`) on KDE/X11 with Fcitx 5.1.12:

| Application | Toolkit | SurroundingText | Input path | Underline |
| --- | --- | --- | --- | --- |
| KWrite | Qt | Available on first key | Direct | None |
| Zenity text entry | GTK4 | Available on first key | Direct | None |
| xterm | XIM | Not available | Client preedit | Yes |
| Firefox textarea | Gecko | Not available | Client preedit | Yes |
| Firefox contenteditable | Gecko | Not available | Client preedit | Yes |
| Firefox address bar | Gecko | Not available | Client preedit | Yes |
| Chrome textarea | Blink | Not available | Client preedit | Yes |
| Chrome contenteditable | Blink | Not available | Client preedit | Yes |
| Chrome address bar | Blink | Not available | Client preedit | Yes |
| VSCode editor | Electron | Not available | Client preedit | Yes |
| VSCode integrated terminal | Electron | Not available | Client preedit | Yes |

**Key finding (Debian 13.6 / KDE Plasma / X11 matrix):** Firefox, Chrome, and
Electron/VS Code did not advertise `CapabilityFlag::SurroundingText` on any
context type observed in this environment. Those contexts remained on
client-preedit fallback for the duration of the test session. Other frontend
versions, desktop environments, or Wayland sessions may differ and remain
unverified.

## Why browsers stay on client preedit

The `CapabilityFlag::SurroundingText` is controlled by the application (via
Fcitx's input context protocol) or the IM module (XIM, Wayland text-input).
Browser engines:

- **Firefox:** Uses Gecko's own IM handling. In the tested X11 matrix, Firefox
  did not advertise `SurroundingText`. The GTK IM module used by Firefox on X11
  (`gtk-im-context`) can provide surrounding text only if the widget requests
  it; Firefox's input fields did not do so in this environment. Wayland may
  differ.
- **Chrome:** Uses Aura (Linux) or Ozone (Wayland). On the tested X11 via GTK,
  Chrome's renderer process handled IME independently of the browser window's
  GTK IM context, and `SurroundingText` was not advertised.
- **Electron/VS Code:** Inherits Chromium's IME architecture. Same observed
  behavior as Chrome in the tested environment.

This is not a UniLume limitation in the tested environment; the observed
behavior reflects the frontend API contract as exercised by these applications
on X11. Wayland, different browser versions, or alternative desktop
environments may produce different capability signals and would need separate
validation.

## Zero-preedit research

Three approaches were evaluated:

### A. Direct replacement (requires SurroundingText)

Direct replacement deletes the minimum characters before the cursor and commits
the replacement in one synchronous transaction. It requires valid, unselected
surrounding text with a trustworthy cursor position and enough characters before
the cursor.

**Browser applicability in tested X11 matrix:** In the environments tested,
browsers did not advertise `CapabilityFlag::SurroundingText`, so
`canReplace()` returned `false`. Whether Wayland or future browser versions
expose this capability is not yet known.

### B. Stable prefix commit

This approach commits the part of the preedit that is guaranteed not to change,
reducing the underlined portion. For example, in "tôi" typed as "tooi", the
first character "t" is stable after the second key produces "tô".

**Analysis:** UniKey's composition is not monotonic. Earlier characters can
change based on later input due to:
- Tone relocation: typing "nghieng" → "nghiêng" changes the "e" to "ê"
- Vowel modification: "tooi" → "tôi" changes "oo" to "ô"
- Backspace + recomposition

Determining a stable prefix requires deep engine introspection that the current
`ukinterface` API does not expose. Adding a "prefix stability oracle" would
require either extending the C API or duplicating engine logic — both outside
scope. Without that oracle, any guessed prefix could be incorrect, violating
the primary correctness invariants (no lost, duplicate, or reordered text).

**Verdict:** Not currently implementable safely with the existing engine API.
Deferred for engine-interface research.

### C. Server-side preedit

Server-side preedit moves the preedit rendering from the client (browser) to
Fcitx, removing the underline. UniLume attempted this during previous
validation.

**Result in tested Firefox/X11 reproduction:** A Firefox 1 ms/key burst lost
part of the Vietnamese sentence and the URL prefix. The timing dependency
between server preedit updates and burst key events caused text loss that
could not be resolved without sleep or retry workarounds.

**Verdict:** Rejected for the tested Firefox/X11 reproduction. Server-side
preedit may behave differently on other frontends or compositors and has not
been re-evaluated.

### Summary

| Approach | Safe? | No underline? | Feasible in tested matrix? |
| --- | --- | --- | --- |
| A. Direct replacement | Yes | Yes (works for Qt/GTK) | No for browser contexts tested |
| B. Stable prefix | Uncertain | Partial | Deferred — needs engine API |
| C. Server-side preedit | No (tested Firefox/X11) | Yes | Rejected for tested reproduction |

**Conclusion (tested X11 matrix):** Safe zero-preedit for browser/Electron
frontends is not currently achievable with the frontend capabilities observed
in the tested environment and the current UniKey engine interface. The
client-preedit fallback with underline is retained as the safe default for
all browser/Electron contexts in this matrix. Wayland and other environments
require separate evaluation.

## Deterministic test profiles

The integration harness models the following browser profiles:

| Profile | SurroundingText (observed) | Behavior |
| --- | --- | --- |
| `firefox-x11` | Not advertised in test matrix | Client-preedit fallback, burst-safe |
| `firefox-wayland` | Pending | Wayland session required |
| `chromium-x11` | Not advertised in test matrix | Client-preedit fallback, burst-safe |
| `chromium-wayland` | Pending | Wayland session required |
| `electron-x11` | Not advertised in test matrix | Client-preedit fallback, burst-safe |
| `electron-wayland` | Pending | Wayland session required |

Browser profiles are tested via `PreeditFallbackController` with the full
corpus (Telex composition, URL, email, C++/C# code, punctuation, backspace).
See `tests/integration/browser_capability_tests.cpp` and
`tests/integration/preedit_fallback_tests.cpp`.

## Limitations

- Browser/Electron capability observations are documented for Debian 13.6 / KDE
  Plasma X11 only. Other distributions, desktop environments, compositors, or
  Wayland sessions may differ and remain unverified.
- Wayland browser profiles are marked `pending`; see
  `docs/wayland-validation.md`.
- The policy never hardcodes process names to force a specific path.
- No claims are made about production readiness.
