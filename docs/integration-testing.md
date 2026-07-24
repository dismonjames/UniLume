<!-- SPDX-License-Identifier: GPL-2.0-or-later -->

# Integration testing

UniLume tests the C++23 direct-commit controller without a desktop session.
The deterministic backend uses a virtual event counter; it never sleeps and
does not depend on scheduler timing.

## Build and test

```sh
cmake -S . -B build/integration -DCMAKE_BUILD_TYPE=Debug
cmake --build build/integration --parallel 2
ctest --test-dir build/integration --output-on-failure
```

CTest separates engine regression from these integration suites:

- `integration-immediate`;
- `integration-delayed`;
- `integration-duplicate`;
- `integration-transaction`;
- `integration-preedit-fallback`;
- `integration-mode-policy`;
- `integration-burst`;
- `integration-soak-smoke`.

The harness covers immediate replacement, 1/2/5/10/50-event delay, missing or
stale surrounding text, invalid surrounding UTF-8, cursor mismatch,
delete/commit failure, duplicate and reordered callback, dropped callback,
focus reset, safe preedit fallback, path selection, and bounded burst input.
The fallback suite repeats the Firefox/Chrome corpus to detect duplicated,
lost, or reordered prefixes. Delay is virtual; fault injection is
deterministic and repeatable.

Every suite checks final output, valid UTF-8, bounded/final queue depth, and
the absence of a pending transaction. Duplicate and reordered callbacks must
be rejected by sequence ID. The sustained delayed profile is five virtual
events. Longer delays are exercised as finite bursts because no finite queue
can absorb an indefinitely faster producer.

## Integration benchmark

The benchmark target is optional and uses the normal Release flags:

```sh
cmake -S . -B build/integration-benchmark \
  -DCMAKE_BUILD_TYPE=Release \
  -DUNILUME_BUILD_INTEGRATION_BENCHMARKS=ON
cmake --build build/integration-benchmark --parallel 2

build/integration-benchmark/benchmarks/unilume_integration_benchmark \
  --keys=1000 \
  --profile=all
```

Profiles are `immediate`, `delayed`, and `stale`. A one-million-key soak and
JSON export use:

```sh
build/integration-benchmark/benchmarks/unilume_integration_benchmark \
  --keys=1000000 \
  --profile=all \
  --format=json \
  --output=integration-benchmark-results.json
```

The report includes per-event p50/p95/p99, mean and standard deviation,
throughput, latency drift, queue metrics, transaction counts, current/peak RSS,
RSS checkpoints, checksum, and lost/duplicate/reordered error counters. Local
result files remain untracked.

Allocation count is `not_measured`. RSS is not an allocation count, and
Issue #16 tracks instrumentation that can cover both C and C++ paths without
distorting the hot path.

The integration numbers measure the controller plus deterministic backend,
not GUI rendering, Fcitx IPC, Lotus, or fcitx5-unikey. They do not establish a
performance comparison with another input method.
