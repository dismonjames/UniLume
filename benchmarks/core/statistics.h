// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_result.h"

#include <cstdint>
#include <vector>

namespace unilume::benchmark {

LatencyStatistics calculateStatistics(
    const std::vector<std::uint64_t> &samples);
double calculateStabilityDriftPercent(
    const std::vector<double> &round_means);

} // namespace unilume::benchmark
