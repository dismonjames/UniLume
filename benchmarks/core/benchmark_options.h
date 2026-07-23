// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace unilume::benchmark {

struct BenchmarkOptions {
    bool smoke{};
    bool soak{};
    std::size_t iterations{30};
    std::size_t warmup_iterations{5};
    std::uint64_t soak_keys{1'000'000};
    std::string corpus{"all"};
    std::string format{"human"};
    std::string output_path;
};

BenchmarkOptions parseOptions(int argc, char **argv);
void printUsage();

} // namespace unilume::benchmark
