// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

#include <array>

namespace unilume::test {

void runInputTests(EngineFixture &engine, TestContext &context)
{
    static constexpr std::array vectors{
        RegressionVector{"empty input", UkTelex, "", ""},
        RegressionVector{"plain ASCII", UkTelex, "hello 123", "hello 123"},
        // CJK byte corruption is tracked separately in Issue #7.
        RegressionVector{"non-Vietnamese UTF-8", UkTelex, "Ελληνικά", "Ελληνικά"},
        RegressionVector{
            "moderately long input",
            UkTelex,
            "abc 123, demo value; abc 123, demo value; "
            "abc 123, demo value; abc 123, demo value;",
            "abc 123, demo value; abc 123, demo value; "
            "abc 123, demo value; abc 123, demo value;",
        },
    };
    context.runVectors(engine, vectors);
}

} // namespace unilume::test
