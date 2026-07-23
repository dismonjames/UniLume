// SPDX-License-Identifier: GPL-2.0-or-later

#include "statistics.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace unilume::benchmark {
namespace {

double percentile(const std::vector<std::uint64_t> &sorted, double fraction)
{
    const double position = fraction * static_cast<double>(sorted.size() - 1);
    const auto lower = static_cast<std::size_t>(position);
    const auto upper = std::min(lower + 1, sorted.size() - 1);
    const double weight = position - static_cast<double>(lower);
    return static_cast<double>(sorted[lower]) * (1.0 - weight) +
           static_cast<double>(sorted[upper]) * weight;
}

double meanRange(const std::vector<double> &values,
                 std::size_t begin,
                 std::size_t end)
{
    return std::accumulate(values.begin() + static_cast<std::ptrdiff_t>(begin),
                           values.begin() + static_cast<std::ptrdiff_t>(end),
                           0.0) /
           static_cast<double>(end - begin);
}

} // namespace

LatencyStatistics calculateStatistics(
    const std::vector<std::uint64_t> &samples)
{
    if (samples.empty()) {
        return {};
    }

    std::vector<std::uint64_t> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    const double sum =
        std::accumulate(sorted.begin(), sorted.end(), 0.0);
    const double mean = sum / static_cast<double>(sorted.size());
    double squared_difference = 0.0;
    for (const std::uint64_t sample : sorted) {
        const double difference = static_cast<double>(sample) - mean;
        squared_difference += difference * difference;
    }

    return {
        sorted.size(),
        static_cast<double>(sorted.front()),
        static_cast<double>(sorted.back()),
        mean,
        std::sqrt(squared_difference / static_cast<double>(sorted.size())),
        percentile(sorted, 0.50),
        percentile(sorted, 0.95),
        percentile(sorted, 0.99),
    };
}

double calculateStabilityDriftPercent(
    const std::vector<double> &round_means)
{
    if (round_means.size() < 2) {
        return 0.0;
    }
    const std::size_t midpoint = round_means.size() / 2;
    const double first = meanRange(round_means, 0, midpoint);
    const double second =
        meanRange(round_means, midpoint, round_means.size());
    if (first == 0.0) {
        return 0.0;
    }
    return ((second - first) / first) * 100.0;
}

} // namespace unilume::benchmark
