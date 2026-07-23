# Core benchmark methodology

UniLume includes a dependency-free C++23 harness for measuring the inherited
engine through the real `ukinterface` API. It does not benchmark a desktop
backend, Lotus, or fcitx5-unikey.

## Build

Performance reports must use the normal Release configuration:

```sh
cmake -S . -B build/benchmarks \
  -DCMAKE_BUILD_TYPE=Release \
  -DUNILUME_BUILD_BENCHMARKS=ON
cmake --build build/benchmarks --parallel 2
ctest --test-dir build/benchmarks --output-on-failure
```

`UNILUME_BUILD_BENCHMARKS` defaults to `OFF`. A normal build does not compile
the harness or read the corpus files, and no benchmark dependency is
downloaded or linked into `unilume_engine`.

## Run

Run all latency, throughput, and burst cases:

```sh
build/benchmarks/benchmarks/unilume_core_benchmark
```

Select one corpus or change the measured rounds:

```sh
build/benchmarks/benchmarks/unilume_core_benchmark \
  --corpus=telex \
  --warmup=10 \
  --iterations=100
```

Corpus names are `telex`, `vni`, `viqr`, `urls_and_emails`, `code_like`,
`unicode`, and `mixed`. Burst cases contain exactly 10, 50, 100, and 1,000 key
events without sleeps.

Export machine-readable JSON:

```sh
build/benchmarks/benchmarks/unilume_core_benchmark \
  --format=json \
  --output=benchmark-results.json
```

Local result files named `benchmark-results*.json` or
`benchmark-results*.csv` are ignored by Git.

## Long-running RSS check

The default long run processes at least one million key events:

```sh
build/benchmarks/benchmarks/unilume_core_benchmark \
  --soak \
  --keys=1000000 \
  --format=json \
  --output=benchmark-results-soak.json
```

The soak result records current RSS from `/proc/self/status`, process maximum
RSS from `getrusage`, ten current-RSS checkpoints, checksum, error count, and
latency drift between the first and second half of checkpoint means. Warm-up
happens before the first checkpoint. The harness reports a linear-growth error
only when growth is both material (at least 1 MiB or 25% of warm RSS) and at
least 80% of checkpoint transitions increase. This is a leak signal, not a
portable absolute memory limit. A full soak also reports a latency-growth
error only when at least 80% of checkpoint transitions increase and the second
half mean is over 25% above the first. CI smoke reports drift without applying
that latency rule.

## Measurement boundaries

- `std::chrono::steady_clock` surrounds only `UnikeyFilter`,
  `UnikeyBackspacePress`, or `UnikeyResetBuf`.
- Corpus loading, setup, output mutation, validation, checksum, reporting, and
  console/file I/O are outside each per-key sample.
- Latency reports min, max, arithmetic mean, population standard deviation,
  interpolated p50/p95/p99, sample count, and drift between the first and
  second half of measured round means.
- Throughput sums measured engine-call nanoseconds and reports keys/second,
  corpus rounds/second, total keys, total time, and an FNV-1a checksum.
- Every measured scenario is checked against an explicit expected output and
  strict UTF-8 validation. It runs twice before measurement to detect
  non-deterministic output, then validates every measured iteration.
- UTF-8 input is counted as the byte events accepted by the current C API, not
  as Unicode scalar values.

The URL and code corpora freeze current Telex core output, including
composition inside some strings. They are not claims that UniLume implements a
URL mode or code mode.

## Sanitizer smoke

The `Benchmark Smoke` workflow builds the harness in Debug with ASan and UBSan
and runs all corpora, burst sizes, and a 10,000-key soak. It validates
correctness and memory safety only. It has no latency or throughput threshold
and is not a required performance gate.

The equivalent local command is:

```sh
cmake -S . -B build/benchmark-smoke \
  -DCMAKE_BUILD_TYPE=Debug \
  -DUNILUME_BUILD_BENCHMARKS=ON \
  -DUNILUME_ENABLE_ASAN=ON \
  -DUNILUME_ENABLE_UBSAN=ON
cmake --build build/benchmark-smoke --parallel 2
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
build/benchmark-smoke/benchmarks/unilume_core_benchmark --smoke
```

## Allocation status and comparison limits

The report deliberately says `allocation_measurement: not_measured`. Counting
only C++ `operator new` would omit C allocation paths and mix harness/report
allocation with engine allocation. RSS is not an allocation count. Reliable
allocation-per-key instrumentation is tracked separately.

Nanosecond results are sensitive to CPU frequency, scheduler activity,
thermal state, compiler, and operating system. Compare results only on the
same machine with the same commit, corpus, compiler, build type, options, and
measurement protocol. A Debug or sanitizer run is not a performance baseline.
Core-only numbers cannot support claims that UniLume is faster than Lotus or
fcitx5-unikey; those projects require direct, equivalent measurements.
