// SPDX-License-Identifier: GPL-2.0-or-later

#include "integration_fixture.h"
#include "test_assertions.h"
#include "test_suites.h"

#include <array>

namespace unilume::integration::test {

void runTransactionTests(Assertions &assertions)
{
    static constexpr std::array<BackendProfile, 4> unsafe_profiles{{
        {.surrounding_text_available = false},
        {.stale_surrounding_text = true},
        {.invalid_surrounding_text = true},
        {.cursor_misaligned = true},
    }};
    for (const BackendProfile profile : unsafe_profiles) {
        IntegrationFixture fixture{profile};
        fixture.type("tooi");
        fixture.drain();
        assertions.equal(
            "unsafe surrounding text uses raw fallback",
            fixture.output(),
            "tooi");
        assertions.truth(
            "unsafe surrounding text resets",
            fixture.metrics().reset_count != 0);
        assertions.equal(
            "unsafe surrounding queue drains",
            fixture.metrics().queue_depth,
            0);
    }

    IntegrationFixture delete_failure{{.fail_next_delete = true}};
    delete_failure.type("tooi");
    delete_failure.drain();
    assertions.equal(
        "delete failure raw fallback", delete_failure.output(), "tooi");
    assertions.truth(
        "delete failure abort recorded",
        delete_failure.metrics().aborted_transactions != 0);

    IntegrationFixture commit_failure{{.fail_next_commit = true}};
    commit_failure.type("abc");
    commit_failure.drain();
    assertions.equal(
        "commit failure raw fallback", commit_failure.output(), "abc");

    IntegrationFixture dropped{
        {.delay_events = 5, .drop_next_callback = true}};
    dropped.type("tieengs");
    dropped.drain();
    assertions.equal("dropped callback recovery", dropped.output(), "tieengs");
    assertions.equal(
        "dropped callback final queue", dropped.metrics().queue_depth, 0);
    assertions.truth(
        "dropped callback final transaction",
        !dropped.metrics().active_transaction);

    IntegrationFixture focus{{.delay_events = 10}};
    focus.type("t");
    focus.focusChange();
    focus.type("tieengs");
    focus.drain();
    assertions.equal(
        "focus reset isolates pending text", focus.output(), "ttiếng");
    assertions.truth(
        "focus reset recorded", focus.metrics().reset_count != 0);
}

} // namespace unilume::integration::test
