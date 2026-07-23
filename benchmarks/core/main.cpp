// SPDX-License-Identifier: GPL-2.0-or-later

#include "benchmark_options.h"
#include "checksum.h"
#include "corpus_loader.h"
#include "engine_fixture.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {

int runCorrectness(const unilume::benchmark::BenchmarkOptions &options)
{
    using namespace unilume::benchmark;

    const auto corpora =
        loadCorpora(std::filesystem::path{UNILUME_BENCHMARK_CORPUS_DIR},
                    options.corpus);
    EngineFixture engine;
    Checksum checksum;
    std::size_t scenarios = 0;
    std::size_t errors = 0;
    std::uint64_t keys = 0;

    for (const Corpus &corpus : corpora) {
        for (const Scenario &scenario : corpus.scenarios) {
            const RunObservation first = engine.run(scenario, false);
            const RunObservation second = engine.run(scenario, false);
            if (first.output != scenario.expected_output ||
                second.output != scenario.expected_output ||
                first.output != second.output) {
                std::cerr << "output mismatch: " << corpus.name << '/'
                          << scenario.name << " expected ["
                          << scenario.expected_output << "] got ["
                          << first.output << "]\n";
                ++errors;
            }
            if (!isValidUtf8(first.output)) {
                std::cerr << "invalid UTF-8 output: " << corpus.name << '/'
                          << scenario.name << '\n';
                ++errors;
            }
            checksum.add(first.output);
            checksum.add(scenarios);
            keys += scenario.events.size();
            ++scenarios;
        }
    }
    if (errors != 0) {
        throw std::runtime_error(std::to_string(errors) +
                                 " correctness invariant(s) failed");
    }

    std::cout << "UniLume core benchmark correctness pass\n"
              << "commit: " << UNILUME_BENCHMARK_COMMIT << '\n'
              << "compiler: " << UNILUME_BENCHMARK_COMPILER << '\n'
              << "build_type: " << UNILUME_BENCHMARK_BUILD_TYPE << '\n'
              << "scenarios: " << scenarios << '\n'
              << "keys: " << keys << '\n'
              << "checksum: " << checksum.value() << '\n';
    return EXIT_SUCCESS;
}

} // namespace

int main(int argc, char **argv)
{
    try {
        return runCorrectness(unilume::benchmark::parseOptions(argc, argv));
    } catch (const std::exception &error) {
        std::cerr << "benchmark error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
