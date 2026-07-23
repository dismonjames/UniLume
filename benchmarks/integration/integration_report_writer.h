// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "integration_benchmark_result.h"

#include <ostream>
#include <string_view>

namespace unilume::integration_benchmark {

void writeReport(const IntegrationReport &report,
                 std::string_view format,
                 std::ostream &output);

} // namespace unilume::integration_benchmark
