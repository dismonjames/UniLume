// SPDX-License-Identifier: GPL-2.0-or-later

#include "integration_benchmark_options.h"
#include "integration_benchmark_runner.h"
#include "integration_report_writer.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    try {
        const auto options =
            unilume::integration_benchmark::parseOptions(argc, argv);
        const auto report =
            unilume::integration_benchmark::runBenchmarks(options);
        if (options.output.empty()) {
            unilume::integration_benchmark::writeReport(
                report, options.format, std::cout);
        } else {
            std::ofstream output{options.output};
            if (!output) {
                throw std::runtime_error("cannot open benchmark output");
            }
            unilume::integration_benchmark::writeReport(
                report, options.format, output);
        }
        for (const auto &result : report.results) {
            if (result.errors != 0) {
                return 1;
            }
        }
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "integration benchmark failed: " << error.what() << '\n';
        return 1;
    }
}
