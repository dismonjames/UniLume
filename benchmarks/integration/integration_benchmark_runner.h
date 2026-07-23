// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "integration_benchmark_options.h"
#include "integration_benchmark_result.h"

namespace unilume::integration_benchmark {

IntegrationReport runBenchmarks(const BenchmarkOptions &options);

} // namespace unilume::integration_benchmark
