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
        RegressionVector{"punctuation", UkTelex, ",.;:!?@#$%&*()-_+=/|", ",.;:!?@#$%&*()-_+=/|"},
        RegressionVector{"URL", UkTelex, "http://abc.com/a1", "http://abc.com/a1"},
        RegressionVector{"email", UkTelex, "abc@demo.com", "abc@demo.com"},
        RegressionVector{"code-like input", UkTelex, "int value = 42;", "int value = 42;"},
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
