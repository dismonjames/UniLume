// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_types.h"

#include <string_view>
#include <vector>

namespace unilume::benchmark {

class EngineFixture;

void validateObservation(const Corpus &corpus,
                         const Scenario &scenario,
                         const RunObservation &observation);
void validateOutput(const Corpus &corpus,
                    const Scenario &scenario,
                    std::string_view output);
void validateCorpora(EngineFixture &engine,
                     const std::vector<Corpus> &corpora);

} // namespace unilume::benchmark
