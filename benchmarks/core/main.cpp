// SPDX-License-Identifier: GPL-2.0-or-later

#include "benchmark_options.h"
#include "burst_benchmark.h"
#include "correctness.h"
#include "corpus_loader.h"
#include "engine_fixture.h"
#include "latency_benchmark.h"
#include "report_writer.h"
#include "system_metadata.h"
#include "throughput_benchmark.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace {

int runBenchmarks(const unilume::benchmark::BenchmarkOptions &options)
{
    using namespace unilume::benchmark;

    std::vector<Corpus> corpora =
        loadCorpora(std::filesystem::path{UNILUME_BENCHMARK_CORPUS_DIR},
                    options.corpus);
    EngineFixture engine;
    validateCorpora(engine, corpora);

    BenchmarkReport report{collectReportMetadata(), {}};
    for (const Corpus &corpus : corpora) {
        report.results.push_back(runLatencyBenchmark(engine, corpus, options));
        report.results.push_back(
            runThroughputBenchmark(engine, corpus, options));
    }
    for (const Corpus &burst : makeBurstCorpora()) {
        report.results.push_back(runLatencyBenchmark(engine, burst, options));
    }

    std::ofstream file;
    std::ostream *output = &std::cout;
    if (!options.output_path.empty()) {
        file.open(options.output_path);
        if (!file) {
            throw std::runtime_error("cannot open output file: " +
                                     options.output_path);
        }
        output = &file;
    }
    writeReport(report, options.format, *output);
    return EXIT_SUCCESS;
}

} // namespace

int main(int argc, char **argv)
{
    try {
        return runBenchmarks(unilume::benchmark::parseOptions(argc, argv));
    } catch (const std::exception &error) {
        std::cerr << "benchmark error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
