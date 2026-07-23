// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

#include <array>

namespace unilume::test {

void runVniTests(EngineFixture &engine, TestContext &context)
{
    static constexpr std::array vectors{
        RegressionVector{"VNI basic phrase", UkVni, "tie61ng Vie65t", "tiếng Việt"},
        RegressionVector{"VNI acute", UkVni, "a1", "á"},
        RegressionVector{"VNI grave", UkVni, "a2", "à"},
        RegressionVector{"VNI hook above", UkVni, "a3", "ả"},
        RegressionVector{"VNI tilde", UkVni, "a4", "ã"},
        RegressionVector{"VNI dot below", UkVni, "a5", "ạ"},
        RegressionVector{"VNI circumflex a", UkVni, "a6", "â"},
        RegressionVector{"VNI circumflex e", UkVni, "e6", "ê"},
        RegressionVector{"VNI circumflex o", UkVni, "o6", "ô"},
        RegressionVector{"VNI horn o", UkVni, "o7", "ơ"},
        RegressionVector{"VNI horn u", UkVni, "u7", "ư"},
        RegressionVector{"VNI breve", UkVni, "a8", "ă"},
        RegressionVector{"VNI d stroke", UkVni, "d9", "đ"},
        RegressionVector{"VNI repeated circumflex", UkVni, "a66", "a6"},
        RegressionVector{"VNI repeated horn", UkVni, "o77", "o7"},
        RegressionVector{"VNI repeated breve", UkVni, "a88", "a8"},
        RegressionVector{"VNI repeated d modifier", UkVni, "d99", "d9"},
        RegressionVector{"VNI uppercase", UkVni, "A6 O7 U7 D9", "Â Ơ Ư Đ"},
        RegressionVector{"VNI numbers without context", UkVni, "2026-42", "2026-42"},
    };
    context.runVectors(engine, vectors);
}

} // namespace unilume::test
