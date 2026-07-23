# Architecture

UniLume currently preserves the x-unikey module boundaries instead of
rewriting the input algorithm.

```text
platform adapter (future)
        |
        v
src/ukinterface   C-compatible key-event API
        |
        v
src/ukengine      UniKey Vietnamese input state machine
        |
        +----> src/vnconv   legacy charset and UTF-8 conversion
                     |
                     +----> src/byteio
```

`tests/engine/` exercises the real `ukinterface` API and simulates the small
responsibility a platform adapter has: echo unchanged keys, apply requested
backspaces, and append replacement bytes.

The historical integrations under `src/platform/legacy/` are retained for
provenance and reference but are not built by CMake. They depend on old
X11/GTK2 interfaces and should not be presented as supported UniLume frontends.
`third_party/imdkit/` is bundled third-party X11R6 code with separate notices.

The only supported build artifact today is the static `unilume_engine`
library used by tests. A future Linux adapter should remain a thin consumer of
this API so engine behavior can continue to be tested without X11 or a GUI.

The proposed boundary, event/output model, ownership rules, backend contract,
and staged removal of per-context global state are documented in
[linux-adapter-design.md](linux-adapter-design.md). This is a design proposal,
not an implemented or supported desktop integration.
