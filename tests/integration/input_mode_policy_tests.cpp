// SPDX-License-Identifier: GPL-2.0-or-later

#include "input_mode_policy.h"
#include "test_assertions.h"
#include "test_suites.h"

namespace unilume::integration::test {

void runInputModePolicyTests(Assertions &assertions)
{
    platform::InputModePolicy unavailable_at_start;
    unavailable_at_start.observe(false);
    unavailable_at_start.observe(true);
    assertions.truth(
        "context does not promote after starting on preedit",
        unavailable_at_start.path() == platform::InputPath::preedit);

    platform::InputModePolicy direct_at_start;
    direct_at_start.observe(true);
    assertions.truth(
        "available capability selects direct path initially",
        direct_at_start.path() == platform::InputPath::direct);

    direct_at_start.observe(false);
    assertions.truth(
        "capability loss returns to preedit",
        direct_at_start.path() == platform::InputPath::preedit);
    direct_at_start.observe(true);
    assertions.truth(
        "capability loss disqualifies direct for context lifetime",
        direct_at_start.path() == platform::InputPath::preedit);
}

} // namespace unilume::integration::test
