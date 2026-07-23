// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>

namespace unilume::benchmark {

std::uint64_t currentRssKiB();
std::uint64_t maximumRssKiB();

} // namespace unilume::benchmark
