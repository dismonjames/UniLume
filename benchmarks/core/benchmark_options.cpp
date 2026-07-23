// SPDX-License-Identifier: GPL-2.0-or-later

#include "benchmark_options.h"

#include <charconv>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace unilume::benchmark {
namespace {

std::uint64_t parseUnsigned(std::string_view text, std::string_view option)
{
    std::uint64_t value{};
    const auto result =
        std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) {
        throw std::runtime_error("invalid value for " + std::string(option));
    }
    return value;
}

std::string_view optionValue(std::string_view argument,
                             std::string_view prefix)
{
    if (!argument.starts_with(prefix)) {
        return {};
    }
    return argument.substr(prefix.size());
}

} // namespace

BenchmarkOptions parseOptions(int argc, char **argv)
{
    BenchmarkOptions options;
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument{argv[index]};
        if (argument == "--smoke") {
            options.smoke = true;
            options.iterations = 2;
            options.warmup_iterations = 1;
            options.soak_keys = 10'000;
        } else if (argument == "--soak") {
            options.soak = true;
        } else if (argument == "--help") {
            printUsage();
            std::exit(0);
        } else if (const auto value = optionValue(argument, "--iterations=");
                   !value.empty()) {
            options.iterations =
                static_cast<std::size_t>(parseUnsigned(value, "--iterations"));
        } else if (const auto value = optionValue(argument, "--warmup=");
                   !value.empty()) {
            options.warmup_iterations =
                static_cast<std::size_t>(parseUnsigned(value, "--warmup"));
        } else if (const auto value = optionValue(argument, "--keys=");
                   !value.empty()) {
            options.soak_keys = parseUnsigned(value, "--keys");
        } else if (const auto value = optionValue(argument, "--corpus=");
                   !value.empty()) {
            options.corpus = value;
        } else if (const auto value = optionValue(argument, "--format=");
                   !value.empty()) {
            options.format = value;
        } else if (const auto value = optionValue(argument, "--output=");
                   !value.empty()) {
            options.output_path = value;
        } else {
            throw std::runtime_error("unknown option: " +
                                     std::string(argument));
        }
    }
    if (options.iterations == 0 || options.soak_keys == 0) {
        throw std::runtime_error("iterations and keys must be greater than 0");
    }
    if (options.format != "human" && options.format != "json") {
        throw std::runtime_error("format must be human or json");
    }
    return options;
}

void printUsage()
{
    std::cout
        << "Usage: unilume_core_benchmark [options]\n"
        << "  --smoke              Run a short correctness benchmark\n"
        << "  --soak               Include the long-running soak benchmark\n"
        << "  --keys=N             Soak key events (default 1000000)\n"
        << "  --iterations=N       Measured corpus rounds (default 30)\n"
        << "  --warmup=N           Warm-up rounds (default 5)\n"
        << "  --corpus=NAME        all, telex, vni, viqr, urls_and_emails,\n"
        << "                       code_like, unicode, or mixed\n"
        << "  --format=human|json  Output format\n"
        << "  --output=PATH        Write report to a file\n";
}

} // namespace unilume::benchmark
