// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "benchmark_types.h"

#include <vector>

namespace unilume::benchmark {

class EngineFixture;

void validateObservation(const Corpus &corpus,
                         const Scenario &scenario,
                         const RunObservation &observation);
void validateCorpora(EngineFixture &engine,
                     const std::vector<Corpus> &corpora);

} // namespace unilume::benchmark
