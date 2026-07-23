// SPDX-License-Identifier: GPL-2.0-or-later

#include "soak_benchmark.h"

#include "checksum.h"
#include "correctness.h"
#include "engine_fixture.h"
#include "rss_reader.h"
#include "statistics.h"

#include <algorithm>
#include <cstddef>
#include <numeric>

namespace unilume::benchmark {
namespace {

bool detectsLinearGrowth(const RssMetrics &rss)
{
    if (rss.checkpoints.size() < 3 ||
        rss.final_kib <= rss.after_warmup_kib) {
        return false;
    }
    const std::uint64_t growth = rss.final_kib - rss.after_warmup_kib;
    const std::uint64_t material_growth =
        std::max<std::uint64_t>(1024, rss.after_warmup_kib / 4);
    if (growth < material_growth) {
        return false;
    }

    std::size_t increasing_steps = 0;
    std::uint64_t previous = rss.after_warmup_kib;
    for (const RssCheckpoint &checkpoint : rss.checkpoints) {
        if (checkpoint.current_kib > previous) {
            ++increasing_steps;
        }
        previous = checkpoint.current_kib;
    }
    return increasing_steps * 5 >= rss.checkpoints.size() * 4;
}

bool detectsLatencyGrowth(const std::vector<double> &checkpoint_means)
{
    if (checkpoint_means.size() < 6) {
        return false;
    }
    std::size_t increasing_steps = 0;
    for (std::size_t index = 1; index < checkpoint_means.size(); ++index) {
        if (checkpoint_means[index] > checkpoint_means[index - 1]) {
            ++increasing_steps;
        }
    }
    const std::size_t midpoint = checkpoint_means.size() / 2;
    const double first_half =
        std::accumulate(checkpoint_means.begin(),
                        checkpoint_means.begin() +
                            static_cast<std::ptrdiff_t>(midpoint),
                        0.0) /
        static_cast<double>(midpoint);
    const double second_half =
        std::accumulate(checkpoint_means.begin() +
                            static_cast<std::ptrdiff_t>(midpoint),
                        checkpoint_means.end(),
                        0.0) /
        static_cast<double>(checkpoint_means.size() - midpoint);
    return second_half > first_half * 1.25 &&
           increasing_steps * 5 >=
               (checkpoint_means.size() - 1) * 4;
}

} // namespace

BenchmarkResult runSoakBenchmark(EngineFixture &engine,
                                 const std::vector<Corpus> &corpora,
                                 const BenchmarkOptions &options)
{
    BenchmarkResult result;
    result.name = "soak/" + options.corpus;
    result.has_rss = true;
    result.rss.initial_kib = currentRssKiB();

    for (const Corpus &corpus : corpora) {
        for (const Scenario &scenario : corpus.scenarios) {
            engine.runAggregate(scenario);
            validateOutput(corpus, scenario, engine.output());
        }
    }
    result.rss.after_warmup_kib = currentRssKiB();

    constexpr std::uint64_t checkpoint_count = 10;
    const std::uint64_t checkpoint_interval =
        std::max<std::uint64_t>(1, options.soak_keys / checkpoint_count);
    std::uint64_t next_checkpoint = checkpoint_interval;
    std::uint64_t checkpoint_nanoseconds = 0;
    std::uint64_t checkpoint_keys = 0;
    std::vector<double> checkpoint_means;
    Checksum checksum;

    while (result.total_keys < options.soak_keys) {
        for (const Corpus &corpus : corpora) {
            for (const Scenario &scenario : corpus.scenarios) {
                const AggregateObservation observation =
                    engine.runAggregate(scenario);
                validateOutput(corpus, scenario, engine.output());
                result.total_keys += observation.events;
                result.total_seconds +=
                    static_cast<double>(observation.total_latency_ns) / 1.0e9;
                checkpoint_nanoseconds += observation.total_latency_ns;
                checkpoint_keys += observation.events;
                checksum.add(engine.output());
                checksum.add(result.iterations);
                ++result.iterations;

                if (result.total_keys >= next_checkpoint) {
                    result.rss.checkpoints.push_back(
                        {result.total_keys, currentRssKiB()});
                    checkpoint_means.push_back(
                        static_cast<double>(checkpoint_nanoseconds) /
                        static_cast<double>(checkpoint_keys));
                    checkpoint_nanoseconds = 0;
                    checkpoint_keys = 0;
                    next_checkpoint += checkpoint_interval;
                }
                if (result.total_keys >= options.soak_keys) {
                    break;
                }
            }
            if (result.total_keys >= options.soak_keys) {
                break;
            }
        }
    }

    result.rss.final_kib = currentRssKiB();
    result.rss.maximum_kib =
        std::max({maximumRssKiB(),
                  result.rss.initial_kib,
                  result.rss.after_warmup_kib,
                  result.rss.final_kib});
    for (const RssCheckpoint &checkpoint : result.rss.checkpoints) {
        result.rss.maximum_kib =
            std::max(result.rss.maximum_kib, checkpoint.current_kib);
    }
    result.rss.linear_growth_detected = detectsLinearGrowth(result.rss);
    result.latency_growth_detected =
        !options.smoke && detectsLatencyGrowth(checkpoint_means);
    result.errors = (result.rss.linear_growth_detected ||
                     result.latency_growth_detected)
                        ? 1
                        : 0;
    result.keys_per_second =
        static_cast<double>(result.total_keys) / result.total_seconds;
    result.iterations_per_second =
        static_cast<double>(result.iterations) / result.total_seconds;
    result.checksum = checksum.value();
    result.latency_stability_drift_percent =
        calculateStabilityDriftPercent(checkpoint_means);
    return result;
}

} // namespace unilume::benchmark
