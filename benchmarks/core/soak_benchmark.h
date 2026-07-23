// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_options.h"
#include "benchmark_result.h"
#include "benchmark_types.h"

#include <vector>

namespace unilume::benchmark {

class EngineFixture;

BenchmarkResult runSoakBenchmark(EngineFixture &engine,
                                 const std::vector<Corpus> &corpora,
                                 const BenchmarkOptions &options);

} // namespace unilume::benchmark
