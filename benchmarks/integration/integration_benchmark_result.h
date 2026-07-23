// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_result.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace unilume::integration_benchmark {

struct IntegrationResult {
    std::string name;
    std::uint64_t total_keys{};
    double total_seconds{};
    double keys_per_second{};
    benchmark::LatencyStatistics latency;
    double latency_drift_percent{};
    std::uint64_t checksum{};
    std::uint64_t errors{};
    std::uint64_t lost_events{};
    std::uint64_t duplicate_events{};
    std::uint64_t reordered_events{};
    std::size_t max_queue_depth{};
    std::size_t final_queue_depth{};
    std::uint64_t completed_transactions{};
    std::uint64_t aborted_transactions{};
    std::uint64_t stale_callbacks{};
    std::uint64_t duplicate_preventions{};
    std::uint64_t reset_count{};
    std::uint64_t queue_overflow_count{};
    bool pending_transaction{};
    benchmark::RssMetrics rss;
};

struct IntegrationReport {
    benchmark::ReportMetadata metadata;
    std::string allocation_measurement{"not_measured"};
    std::vector<IntegrationResult> results;
};

} // namespace unilume::integration_benchmark
