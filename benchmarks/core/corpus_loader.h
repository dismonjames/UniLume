// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_types.h"

#include <filesystem>
#include <string_view>
#include <vector>

namespace unilume::benchmark {

std::vector<Corpus> loadCorpora(const std::filesystem::path &directory,
                                std::string_view selection);

} // namespace unilume::benchmark
