<!-- SPDX-License-Identifier: GPL-2.0-or-later -->

# Minimal Linux adapter design

Status: proposal for Issue #4. This document defines boundaries and follow-up
work; it does not claim that a Linux backend is implemented or supported.

## 1. Goals

- Keep the inherited UniKey engine isolated and behavior-compatible.
- Put new lifecycle and policy code in C++23.
- Expose a small C ABI with explicit buffers, ownership, and errors.
- Allow desktop backends to be replaced without changing engine tests.
- Support deterministic tests and measurement without a real desktop session.

## 2. Non-goals

- Implementing X11, Wayland, Fcitx5, IBus, a daemon, or a GUI.
- Selecting the first production input-method framework.
- Rewriting the UniKey algorithm or restoring historical XIM/GTK2 builds.
- Claiming compositor-independent, system-wide Wayland key interception.

## 3. Technical constraints

The legacy sources in `src/ukengine/`, `src/ukinterface/`, `src/vnconv/`, and
`src/byteio/` retain their current dialect and module boundaries. New code is
C++23. STL types, C++ exceptions, RTTI-dependent types, and ownership-bearing
C++ objects must not cross the C ABI.

The current `ukinterface` API uses process-global mutable state. It is useful
for compatibility and tests but cannot represent independent application
focus contexts safely. Multi-context support must not be simulated by
switching global buffers behind callers.

## 4. Proposed architecture

```text
desktop framework
      |
      v
platform backend          framework-specific event and surrounding-text I/O
      |
      v
adapter session           focus lifecycle, edit routing, bounded preedit mirror
      |
      v
C++23 engine wrapper      RAII, validation, POD/C ABI translation
      |
      v
opaque C engine context   stable functions and fixed-capacity output records
      |
      v
legacy UniKey engine      existing composition algorithm
```

The adapter session depends on a narrow backend sink. The backend depends on
its desktop framework, but neither the wrapper nor engine includes desktop
headers.

## 5. C++98 legacy to C++23 boundary

The existing legacy directories remain in place. Proposed new production
locations are:

```text
src/
├── byteio/          # inherited
├── ukengine/        # inherited
├── ukinterface/     # inherited API plus future opaque C context facade
├── vnconv/          # inherited
├── core/            # C++23 RAII wrapper and adapter session
└── platform/        # one subdirectory per implemented backend
```

No empty directory should be created before its implementation Issue. The
historical sources remain under `src/platform/legacy/` and are not a base
class for new backends.

## 6. C ABI

The wrapper initially calls the existing functions:

- `UnikeySetup` / `UnikeyCleanup`;
- `UnikeySetInputMethod`, `UnikeySetOptions`, `UnikeySetCapsState`;
- `UnikeyFilter`, `UnikeyBackspacePress`, `UnikeyResetBuf`;
- `UnikeyBackspaces`, `UnikeyBufChars`, and `UnikeyBuf`.

A separate implementation milestone should introduce an opaque handle rather
than extend global variables:

```c
typedef struct UlEngineContext UlEngineContext;

UlStatus ul_engine_create(const UlConfig *config, UlEngineContext **out);
void ul_engine_destroy(UlEngineContext *context);
UlStatus ul_engine_filter(
    UlEngineContext *context,
    const UlKeyInput *input,
    UlEdit *edit);
void ul_engine_reset(UlEngineContext *context);
```

The exact names are provisional. The ABI uses fixed-width integers, pointers
plus lengths, caller-provided output storage, and status codes.

## 7. Key-event flow

1. A backend translates a framework event into `KeyInput`.
2. The adapter ignores key releases and unsupported control events.
3. Navigation, focus loss, and external cursor movement reset the context.
4. Non-ASCII text that is not a Vietnamese key action is preserved.
5. ASCII composition keys are sent through the engine boundary.
6. The wrapper converts engine backspaces and output bytes into one `Edit`.
7. The adapter applies delete, commit, and optional preedit updates in order.
8. On backend failure, the context is reset before processing another key.

## 8. Input event model

The C-facing input is a POD record containing a Unicode scalar value or ASCII
key value, normalized modifiers, press/repeat flags, and an optional UTF-8
slice. Physical scan codes and framework object pointers remain backend-local.
Lengths are explicit; no NUL termination is assumed.

Modifier normalization must distinguish Shift/Caps state used by UniKey from
Ctrl/Alt/Super shortcuts that should bypass composition.

## 9. Output model

One engine call produces a bounded edit record:

- number of Unicode code points to delete before the cursor;
- byte count and caller-owned capacity for replacement UTF-8;
- flags describing commit, preedit refresh, reset, or passthrough;
- an error/status code.

The record never owns memory and remains valid only for the call.

## 10. Preedit

The legacy API describes edits to already-visible text rather than a native
preedit protocol. The first adapter may use direct commit plus surrounding
deletion. A bounded preedit mirror is optional and must be proven equivalent
with regression tests before being enabled.

Backends that cannot delete surrounding text must not silently advertise full
support; they may use framework preedit when its semantics are verified.

## 11. Commit text

The adapter commits exactly the UTF-8 bytes produced by the wrapper or the
original passthrough input. UTF-8 validation happens before calling a backend.
No backend-specific string type crosses into core.

## 12. Backspace and surrounding deletion

`UnikeyBackspaces` counts logical characters, not UTF-8 bytes. The adapter
requests deletion before the cursor in code points. If the framework cannot
confirm deletion, the adapter resets and returns a recoverable error instead
of guessing byte offsets.

Physical Backspace calls the engine backspace function only while a tracked
composition context exists; otherwise the backend receives the original key.

## 13. Reset context

Reset occurs on focus change, cursor movement, selection change, input-method
switch, unrecoverable backend error, and non-composable input that ends the
current word. Reset clears engine state and the adapter's preedit mirror.

## 14. Ownership and lifetime

One adapter session belongs to one framework input context or focused client,
not to the whole desktop. The C++23 wrapper uniquely owns one opaque engine
context through RAII. Backend references are non-owning and cannot outlive
their session. Edit buffers are owned by the session and reused.

## 15. Error handling

C functions return a small enum such as `ok`, `invalid_input`,
`output_too_small`, `unsupported`, and `internal_error`. They do not throw.
The C++ wrapper may use internal C++ error types but catches all exceptions
before a callback or C boundary. Recoverable failures reset composition and
let the backend forward the original key.

## 16. Threading model

Each context is thread-confined to the framework event thread. Concurrent
calls on one context are invalid. Different contexts may run concurrently
only after the opaque-context implementation removes legacy global mutable
state. The first implementation must document single-thread limitations
rather than add a global lock and imply multi-context correctness.

## 17. Configuration boundary

Configuration is an immutable POD snapshot copied when a context is created
or explicitly reconfigured. It contains input method, output encoding, and
documented boolean options. File paths use pointer-plus-length views and are
copied by the owning C++ layer; the engine never retains caller pointers.

## 18. Backend interface

The C++23 adapter needs only a small backend sink:

- delete surrounding text before the cursor;
- commit UTF-8;
- set or clear preedit when supported;
- forward an unhandled key;
- report surrounding-text and capability availability.

Framework-specific candidates implement this contract independently. The
contract does not mention Fcitx5, IBus, X11, or Wayland types.

## 19. Test strategy

- Keep engine regression tests linked only to `ukinterface`.
- Test the C++23 wrapper with the real C ABI and fixed test buffers.
- Test adapter sessions against a recording fake backend.
- Use contract tests for deletion/commit ordering, reset, focus changes,
  Unicode passthrough, buffer exhaustion, and backend failure.
- Run backend integration tests only when the relevant framework is present.

## 20. Sanitizer and fuzzer strategy

Run ASan and UBSan on engine/wrapper contract tests in dedicated CI jobs after
known legacy findings are triaged. Fuzz the opaque C function with bounded
sequences of key inputs, Backspace, reset, and method changes. Seed corpora
come from Telex/VNI/VIQR regressions and malformed UTF-8 cases.

Fuzzers must cap sequence length and output capacity and must not invoke a
desktop backend.

## 21. Benchmark strategy

Benchmark warmed single-key latency, percentile latency over word sequences,
and throughput over deterministic corpora. Measure engine-only, wrapper, and
fake-backend paths separately. Pin configuration outside the timed loop and
reuse buffers so allocation is visible rather than hidden.

## 22. Security considerations

- Validate scalar values, UTF-8, lengths, and output capacity.
- Treat surrounding text as untrusted and bound every copy.
- Never build shell commands from committed text or configuration paths.
- Clear composition on focus changes to prevent text crossing clients.
- Do not log typed text by default.
- Keep legacy XIM install scripts outside runtime and packaging paths.

## 23. Performance considerations

Use fixed-capacity edit and preedit buffers sized from measured engine output.
Reuse them for each key. Avoid heap allocation, locale conversion, virtual
dispatch chains, and configuration parsing in the key-event path. An explicit
`output_too_small` result is preferable to silent truncation.

## 24. Packaging implications

The engine/core library should have no X11, Wayland, GTK, Fcitx, or IBus
runtime dependency. Each implemented backend can be an optional package with
its framework dependency and metadata. No end-user package should be
published until an executable or framework module and its lifecycle are
implemented and tested.

## 25. Rejected alternatives

- **Modernize the engine in place:** high regression and provenance risk.
- **Expose C++ STL across the boundary:** unstable ABI and ownership.
- **Keep one global engine for every client:** focus leakage and races.
- **Build one universal desktop backend:** couples unrelated protocols.
- **Restore legacy XIM/GTK2 as the new platform layer:** obsolete dependencies
  and misleading support claims.
- **Add a daemon first:** expands lifecycle, IPC, and security scope without a
  demonstrated need.

## 26. Open questions

- Which maintained framework should be evaluated first: Fcitx5 or IBus?
- Which frameworks guarantee surrounding-text deletion semantics?
- Is native preedit required for the first usable milestone?
- What fixed output capacity covers measured worst cases?
- How should multiple seats and concurrent application contexts map to engine
  contexts?
- Should invalid UTF-8 be forwarded, rejected, or replaced at the backend
  boundary?
- Which legacy global tables are immutable after initialization and can be
  shared safely?

## 27. Proposed implementation milestones

1. Specify opaque C context structs and add contract tests; no backend.
2. Remove per-context dependence on mutable globals without algorithm changes.
3. Implement the C++23 RAII wrapper and recording fake backend tests.
4. Implement adapter-session edit ordering, reset, and capability handling.
5. Benchmark and fuzz the engine/wrapper boundary.
6. Evaluate Fcitx5 and IBus in separate proposal Issues.
7. Implement one experimental backend only after maintainer selection.
8. Add packaging and end-user lifecycle documentation after integration tests.
