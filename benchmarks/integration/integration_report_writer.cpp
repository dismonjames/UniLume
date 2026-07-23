// SPDX-License-Identifier: GPL-2.0-or-later

#include "integration_report_writer.h"

#include <iomanip>

namespace unilume::integration_benchmark {
namespace {

void writeHuman(const IntegrationReport &report, std::ostream &output)
{
    output << "UniLume integration benchmark\n"
           << "commit: " << report.metadata.commit << '\n'
           << "compiler: " << report.metadata.compiler << '\n'
           << "build_type: " << report.metadata.build_type << '\n'
           << "cpu: " << report.metadata.cpu_model << '\n'
           << "allocation_measurement: "
           << report.allocation_measurement << "\n\n"
           << std::fixed << std::setprecision(2);
    for (const IntegrationResult &result : report.results) {
        output << result.name << '\n'
               << "  keys=" << result.total_keys
               << " seconds=" << result.total_seconds
               << " keys/s=" << result.keys_per_second << '\n'
               << "  latency_ns p50=" << result.latency.p50_ns
               << " p95=" << result.latency.p95_ns
               << " p99=" << result.latency.p99_ns
               << " mean=" << result.latency.mean_ns
               << " drift=" << result.latency_drift_percent << "%\n"
               << "  queue max=" << result.max_queue_depth
               << " final=" << result.final_queue_depth
               << " pending=" << result.pending_transaction << '\n'
               << "  completed=" << result.completed_transactions
               << " aborted=" << result.aborted_transactions
               << " stale=" << result.stale_callbacks
               << " duplicate_prevented="
               << result.duplicate_preventions << '\n'
               << "  RSS KiB initial=" << result.rss.initial_kib
               << " final=" << result.rss.final_kib
               << " peak=" << result.rss.maximum_kib
               << " linear_growth="
               << result.rss.linear_growth_detected << '\n'
               << "  checksum=" << result.checksum
               << " errors=" << result.errors
               << " lost=" << result.lost_events
               << " duplicate=" << result.duplicate_events
               << " reordered=" << result.reordered_events << "\n\n";
    }
}

void writeLatency(const benchmark::LatencyStatistics &latency,
                  std::ostream &output)
{
    output << "{\"samples\":" << latency.samples
           << ",\"min_ns\":" << latency.min_ns
           << ",\"p50_ns\":" << latency.p50_ns
           << ",\"p95_ns\":" << latency.p95_ns
           << ",\"p99_ns\":" << latency.p99_ns
           << ",\"max_ns\":" << latency.max_ns
           << ",\"mean_ns\":" << latency.mean_ns
           << ",\"stddev_ns\":" << latency.stddev_ns << '}';
}

void writeJson(const IntegrationReport &report, std::ostream &output)
{
    output << std::fixed << std::setprecision(9)
           << "{\"metadata\":{\"commit\":\"" << report.metadata.commit
           << "\",\"compiler\":\"" << report.metadata.compiler
           << "\",\"build_type\":\"" << report.metadata.build_type
           << "\",\"cpu_model\":\"" << report.metadata.cpu_model
           << "\",\"timestamp_utc\":\"" << report.metadata.timestamp_utc
           << "\",\"allocation_measurement\":\""
           << report.allocation_measurement << "\"},\"results\":[";
    for (std::size_t index = 0; index < report.results.size(); ++index) {
        const IntegrationResult &result = report.results[index];
        if (index != 0) {
            output << ',';
        }
        output << "{\"name\":\"" << result.name
               << "\",\"total_keys\":" << result.total_keys
               << ",\"total_seconds\":" << result.total_seconds
               << ",\"keys_per_second\":" << result.keys_per_second
               << ",\"latency\":";
        writeLatency(result.latency, output);
        output << ",\"latency_drift_percent\":"
               << result.latency_drift_percent
               << ",\"max_queue_depth\":" << result.max_queue_depth
               << ",\"final_queue_depth\":" << result.final_queue_depth
               << ",\"pending_transaction\":"
               << (result.pending_transaction ? "true" : "false")
               << ",\"completed_transactions\":"
               << result.completed_transactions
               << ",\"aborted_transactions\":"
               << result.aborted_transactions
               << ",\"stale_callbacks\":" << result.stale_callbacks
               << ",\"duplicate_preventions\":"
               << result.duplicate_preventions
               << ",\"queue_overflow_count\":"
               << result.queue_overflow_count
               << ",\"lost_events\":" << result.lost_events
               << ",\"duplicate_events\":" << result.duplicate_events
               << ",\"reordered_events\":" << result.reordered_events
               << ",\"rss\":{\"initial_kib\":" << result.rss.initial_kib
               << ",\"final_kib\":" << result.rss.final_kib
               << ",\"peak_kib\":" << result.rss.maximum_kib
               << ",\"linear_growth_detected\":"
               << (result.rss.linear_growth_detected ? "true" : "false")
               << ",\"checkpoints\":[";
        for (std::size_t checkpoint = 0;
             checkpoint < result.rss.checkpoints.size();
             ++checkpoint) {
            if (checkpoint != 0) {
                output << ',';
            }
            output << "{\"keys\":"
                   << result.rss.checkpoints[checkpoint].keys
                   << ",\"current_kib\":"
                   << result.rss.checkpoints[checkpoint].current_kib << '}';
        }
        output << "]},\"checksum\":" << result.checksum
               << ",\"errors\":" << result.errors << '}';
    }
    output << "]}\n";
}

} // namespace

void writeReport(const IntegrationReport &report,
                 std::string_view format,
                 std::ostream &output)
{
    if (format == "json") {
        writeJson(report, output);
    } else {
        writeHuman(report, output);
    }
}

} // namespace unilume::integration_benchmark
