// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_result.h"

#include <iosfwd>
#include <string_view>

namespace unilume::benchmark {

void writeReport(const BenchmarkReport &report,
                 std::string_view format,
                 std::ostream &output);

} // namespace unilume::benchmark
