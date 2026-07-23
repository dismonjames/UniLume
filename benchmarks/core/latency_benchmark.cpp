// SPDX-License-Identifier: GPL-2.0-or-later

#include "latency_benchmark.h"

#include "checksum.h"
#include "correctness.h"
#include "engine_fixture.h"
#include "statistics.h"

#include <numeric>

namespace unilume::benchmark {

BenchmarkResult runLatencyBenchmark(EngineFixture &engine,
                                    const Corpus &corpus,
                                    const BenchmarkOptions &options)
{
    for (std::size_t iteration = 0;
         iteration < options.warmup_iterations;
         ++iteration) {
        for (const Scenario &scenario : corpus.scenarios) {
            validateObservation(corpus, scenario, engine.run(scenario, false));
        }
    }

    std::vector<std::uint64_t> samples;
    std::vector<double> round_means;
    Checksum checksum;
    std::uint64_t total_keys = 0;
    std::uint64_t total_nanoseconds = 0;

    for (std::size_t iteration = 0; iteration < options.iterations;
         ++iteration) {
        std::uint64_t round_nanoseconds = 0;
        std::uint64_t round_keys = 0;
        for (const Scenario &scenario : corpus.scenarios) {
            const RunObservation observation = engine.run(scenario, true);
            validateObservation(corpus, scenario, observation);
            samples.insert(samples.end(),
                           observation.latency_ns.begin(),
                           observation.latency_ns.end());
            const std::uint64_t scenario_nanoseconds =
                std::accumulate(observation.latency_ns.begin(),
                                observation.latency_ns.end(),
                                std::uint64_t{});
            round_nanoseconds += scenario_nanoseconds;
            round_keys += observation.latency_ns.size();
            checksum.add(observation.output);
            checksum.add(iteration);
        }
        total_nanoseconds += round_nanoseconds;
        total_keys += round_keys;
        round_means.push_back(
            static_cast<double>(round_nanoseconds) /
            static_cast<double>(round_keys));
    }

    const double seconds = static_cast<double>(total_nanoseconds) / 1.0e9;
    BenchmarkResult result;
    result.name = "latency/" + corpus.name;
    result.total_keys = total_keys;
    result.iterations = options.iterations;
    result.total_seconds = seconds;
    result.keys_per_second = static_cast<double>(total_keys) / seconds;
    result.iterations_per_second =
        static_cast<double>(options.iterations) / seconds;
    result.checksum = checksum.value();
    result.latency = calculateStatistics(samples);
    result.latency_stability_drift_percent =
        calculateStabilityDriftPercent(round_means);
    return result;
}

} // namespace unilume::benchmark
