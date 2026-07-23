// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

#include <array>

namespace unilume::test {

void runUrlAndCodeInputTests(EngineFixture &engine, TestContext &context)
{
    static constexpr std::array vectors{
        RegressionVector{
            "Telex URL negative consonant sequence regression",
            UkTelex,
            "http://abc.com/a1",
            "http://abc.com/a1",
        },
        RegressionVector{
            "Telex URL ending in digit",
            UkTelex,
            "http://abc.com/a9",
            "http://abc.com/a9",
        },
        RegressionVector{
            "Telex URL punctuation",
            UkTelex,
            "http://abc.com/a:b/c.d",
            "http://abc.com/a:b/c.d",
        },
        RegressionVector{
            "Telex email with digit",
            UkTelex,
            "abc9@demo.com",
            "abc9@demo.com",
        },
        RegressionVector{
            "Telex code-like identifier and number",
            UkTelex,
            "value_2 = item42;",
            "value_2 = item42;",
        },
        RegressionVector{
            "Telex consecutive punctuation",
            UkTelex,
            ".,:;///@@@",
            ".,:;///@@@",
        },
        RegressionVector{
            "Telex digit to unsupported consonant sequence",
            UkTelex,
            "1http",
            "1http",
        },
        RegressionVector{
            "Telex remains active after URL",
            UkTelex,
            "http://abc.com/a1 tieengs",
            "http://abc.com/a1 tiếng",
        },
        RegressionVector{"Telex control", UkTelex, "tieengs", "tiếng"},
        RegressionVector{"VNI control", UkVni, "tie61ng", "tiếng"},
        RegressionVector{"VIQR control", UkViqr, "tie^'ng", "tiếng"},
        RegressionVector{
            "Moderately long URL and text",
            UkTelex,
            "http://abc.com/path42?q=value then tieengs Vieetj",
            "http://abc.com/path42?q=value then tiếng Việt",
        },
        RegressionVector{"empty URL-like input", UkTelex, "", ""},
    };
    context.runVectors(engine, vectors);

    engine.begin(UkTelex);
    engine.resetContext();
    engine.feed("http://abc.com/a1");
    engine.resetContext();
    engine.feed(" tieengs");
    context.expectEqual(
        "reset context around URL", engine.output(), "http://abc.com/a1 tiếng");

    engine.type("http://abc.com/a1", UkTelex);
    engine.pressBackspace();
    context.expectEqual("backspace after URL", engine.output(), "http://abc.com/a");
}

} // namespace unilume::test
