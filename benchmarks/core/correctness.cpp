// SPDX-License-Identifier: GPL-2.0-or-later

#include "correctness.h"

#include "engine_fixture.h"

#include <stdexcept>

namespace unilume::benchmark {

void validateObservation(const Corpus &corpus,
                         const Scenario &scenario,
                         const RunObservation &observation)
{
    if (observation.output != scenario.expected_output) {
        throw std::runtime_error("output mismatch: " + corpus.name + "/" +
                                 scenario.name + " expected [" +
                                 scenario.expected_output + "] got [" +
                                 observation.output + "]");
    }
    if (!isValidUtf8(observation.output)) {
        throw std::runtime_error("invalid UTF-8 output: " + corpus.name + "/" +
                                 scenario.name);
    }
}

void validateCorpora(EngineFixture &engine,
                     const std::vector<Corpus> &corpora)
{
    for (const Corpus &corpus : corpora) {
        for (const Scenario &scenario : corpus.scenarios) {
            const RunObservation first = engine.run(scenario, false);
            const RunObservation second = engine.run(scenario, false);
            validateObservation(corpus, scenario, first);
            validateObservation(corpus, scenario, second);
            if (first.output != second.output) {
                throw std::runtime_error("non-deterministic output: " +
                                         corpus.name + "/" + scenario.name);
            }
        }
    }
}

} // namespace unilume::benchmark
