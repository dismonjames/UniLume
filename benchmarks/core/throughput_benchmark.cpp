// SPDX-License-Identifier: GPL-2.0-or-later

#include "throughput_benchmark.h"

#include "checksum.h"
#include "correctness.h"
#include "engine_fixture.h"

namespace unilume::benchmark {

BenchmarkResult runThroughputBenchmark(EngineFixture &engine,
                                       const Corpus &corpus,
                                       const BenchmarkOptions &options)
{
    for (std::size_t iteration = 0;
         iteration < options.warmup_iterations;
        ++iteration) {
        for (const Scenario &scenario : corpus.scenarios) {
            engine.runAggregate(scenario);
            validateOutput(corpus, scenario, engine.output());
        }
    }

    Checksum checksum;
    std::uint64_t total_keys = 0;
    std::uint64_t total_nanoseconds = 0;
    for (std::size_t iteration = 0; iteration < options.iterations;
         ++iteration) {
        for (const Scenario &scenario : corpus.scenarios) {
            const AggregateObservation observation =
                engine.runAggregate(scenario);
            validateOutput(corpus, scenario, engine.output());
            total_nanoseconds += observation.total_latency_ns;
            total_keys += observation.events;
            checksum.add(engine.output());
            checksum.add(iteration);
        }
    }

    const double seconds = static_cast<double>(total_nanoseconds) / 1.0e9;
    BenchmarkResult result;
    result.name = "throughput/" + corpus.name;
    result.total_keys = total_keys;
    result.iterations = options.iterations;
    result.total_seconds = seconds;
    result.keys_per_second = static_cast<double>(total_keys) / seconds;
    result.iterations_per_second =
        static_cast<double>(options.iterations) / seconds;
    result.checksum = checksum.value();
    return result;
}

} // namespace unilume::benchmark
