// SPDX-License-Identifier: GPL-2.0-or-later

#include "integration_fixture.h"
#include "test_assertions.h"
#include "test_suites.h"

#include <array>

namespace unilume::integration::test {

void runDelayedTests(Assertions &assertions)
{
    static constexpr std::array<std::size_t, 6> delays{1, 2, 5, 10, 50, 1};
    static constexpr std::array<std::string_view, 6> inputs{
        "tieengs",
        "tooi",
        "tooi ddang gox tieengs vieetj",
        "http://abc.com/a1 tieengs",
        "user9@example.com tieengs",
        "日本語 tieengs",
    };
    static constexpr std::array<std::string_view, 6> expected{
        "tiếng",
        "tôi",
        "tôi đang gõ tiếng việt",
        "http://abc.com/a1 tiếng",
        "ủe9@ẽample.com tiếng",
        "日本語 tiếng",
    };

    for (std::size_t index = 0; index < delays.size(); ++index) {
        IntegrationFixture fixture{{.delay_events = delays[index]}};
        fixture.type(inputs[index]);
        fixture.drain();
        assertions.equal("delayed output", fixture.output(), expected[index]);
        assertions.truth(
            "delayed queue bounded",
            fixture.metrics().max_queue_depth <=
                core::DirectCommitController::queue_capacity);
        assertions.equal(
            "delayed final queue", fixture.metrics().queue_depth, 0);
        assertions.truth(
            "delayed final transaction",
            !fixture.metrics().active_transaction);
    }
}

} // namespace unilume::integration::test
