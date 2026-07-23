// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

#include <array>

namespace unilume::test {

void runViqrTests(EngineFixture &engine, TestContext &context)
{
    static constexpr std::array vectors{
        RegressionVector{"VIQR acute", UkViqr, "a'", "á"},
        RegressionVector{"VIQR grave", UkViqr, "a`", "à"},
        RegressionVector{"VIQR hook above", UkViqr, "a?", "ả"},
        RegressionVector{"VIQR tilde", UkViqr, "a~", "ã"},
        RegressionVector{"VIQR dot below", UkViqr, "a.", "ạ"},
        RegressionVector{"VIQR circumflex a", UkViqr, "a^", "â"},
        RegressionVector{"VIQR breve", UkViqr, "a(", "ă"},
        RegressionVector{"VIQR circumflex o", UkViqr, "o^", "ô"},
        RegressionVector{"VIQR horn o", UkViqr, "o+", "ơ"},
        RegressionVector{"VIQR horn u", UkViqr, "u+", "ư"},
        RegressionVector{"VIQR d stroke", UkViqr, "dD", "đ"},
        RegressionVector{"VIQR repeated modifier", UkViqr, "a^^", "a^"},
        RegressionVector{"VIQR escaped modifier", UkViqr, "a\\^", "a^"},
        RegressionVector{"VIQR escaped tone", UkViqr, "a\\'", "a'"},
        RegressionVector{"VIQR invalid modifier target", UkViqr, "b^", "b^"},
        RegressionVector{"VIQR uppercase", UkViqr, "A^ O+ U+ DD", "Â Ơ Ư Đ"},
    };
    context.runVectors(engine, vectors);
}

} // namespace unilume::test
