// SPDX-License-Identifier: GPL-2.0-or-later

#include "report_writer.h"

#include <iomanip>
#include <ostream>
#include <string>

namespace unilume::benchmark {
namespace {

std::string escapeJson(std::string_view text)
{
    std::string escaped;
    for (const char character : text) {
        switch (character) {
        case '\\':
            escaped += "\\\\";
            break;
        case '"':
            escaped += "\\\"";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped += character;
        }
    }
    return escaped;
}

void writeHuman(const BenchmarkReport &report, std::ostream &output)
{
    output << "UniLume core benchmark\n"
           << "commit: " << report.metadata.commit << '\n'
           << "compiler: " << report.metadata.compiler << '\n'
           << "build_type: " << report.metadata.build_type << '\n'
           << "cpu: " << report.metadata.cpu_model << '\n'
           << "timestamp_utc: " << report.metadata.timestamp_utc << '\n'
           << "allocations: " << report.metadata.allocation_measurement << "\n\n";
    output << std::fixed << std::setprecision(2);
    for (const BenchmarkResult &result : report.results) {
        output << result.name << '\n'
               << "  keys: " << result.total_keys
               << ", iterations: " << result.iterations
               << ", seconds: " << result.total_seconds << '\n'
               << "  keys/s: " << result.keys_per_second
               << ", iterations/s: " << result.iterations_per_second << '\n'
               << "  checksum: " << result.checksum
               << ", errors: " << result.errors << '\n';
        if (result.latency.samples != 0) {
            output << "  latency ns: min=" << result.latency.min_ns
                   << " p50=" << result.latency.p50_ns
                   << " p95=" << result.latency.p95_ns
                   << " p99=" << result.latency.p99_ns
                   << " max=" << result.latency.max_ns
                   << " mean=" << result.latency.mean_ns
                   << " stddev=" << result.latency.stddev_ns
                   << " samples=" << result.latency.samples << '\n'
                   << "  latency drift: "
                   << result.latency_stability_drift_percent << "%\n";
        }
    }
}

void writeLatencyJson(const LatencyStatistics &latency, std::ostream &output)
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

void writeJson(const BenchmarkReport &report, std::ostream &output)
{
    output << std::fixed << std::setprecision(9);
    output << "{\"metadata\":{\"commit\":\""
           << escapeJson(report.metadata.commit) << "\",\"compiler\":\""
           << escapeJson(report.metadata.compiler) << "\",\"build_type\":\""
           << escapeJson(report.metadata.build_type) << "\",\"cpu_model\":\""
           << escapeJson(report.metadata.cpu_model)
           << "\",\"timestamp_utc\":\""
           << escapeJson(report.metadata.timestamp_utc)
           << "\",\"allocation_measurement\":\""
           << escapeJson(report.metadata.allocation_measurement)
           << "\"},\"results\":[";
    for (std::size_t index = 0; index < report.results.size(); ++index) {
        const BenchmarkResult &result = report.results[index];
        if (index != 0) {
            output << ',';
        }
        output << "{\"name\":\"" << escapeJson(result.name)
               << "\",\"total_keys\":" << result.total_keys
               << ",\"iterations\":" << result.iterations
               << ",\"total_seconds\":" << result.total_seconds
               << ",\"keys_per_second\":" << result.keys_per_second
               << ",\"iterations_per_second\":"
               << result.iterations_per_second << ",\"checksum\":"
               << result.checksum << ",\"errors\":" << result.errors
               << ",\"latency_stability_drift_percent\":"
               << result.latency_stability_drift_percent
               << ",\"latency\":";
        writeLatencyJson(result.latency, output);
        output << ",\"rss\":null}";
    }
    output << "]}\n";
}

} // namespace

void writeReport(const BenchmarkReport &report,
                 std::string_view format,
                 std::ostream &output)
{
    if (format == "json") {
        writeJson(report, output);
    } else {
        writeHuman(report, output);
    }
}

} // namespace unilume::benchmark
