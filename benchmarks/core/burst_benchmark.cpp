// SPDX-License-Identifier: GPL-2.0-or-later

#include "burst_benchmark.h"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <utility>

namespace unilume::benchmark {

std::vector<Corpus> makeBurstCorpora()
{
    static constexpr std::array<std::size_t, 4> sizes{10, 50, 100, 1000};
    static constexpr std::string_view pattern{"abc123/.-_"};

    std::vector<Corpus> corpora;
    for (const std::size_t size : sizes) {
        std::string input;
        input.reserve(size);
        while (input.size() < size) {
            const std::size_t remaining = size - input.size();
            input.append(pattern.substr(0, std::min(remaining, pattern.size())));
        }

        Scenario scenario;
        scenario.name = "burst_" + std::to_string(size);
        scenario.expected_output = input;
        scenario.events.reserve(input.size());
        for (const unsigned char key : input) {
            scenario.events.push_back({EventType::key, key});
        }
        corpora.push_back({scenario.name, {std::move(scenario)}});
    }
    return corpora;
}

} // namespace unilume::benchmark
