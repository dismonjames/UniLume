// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

#include <array>

namespace unilume::test {

void runUnicodePassthroughTests(EngineFixture &engine, TestContext &context)
{
    static constexpr std::array vectors{
        RegressionVector{"CJK UTF-8", UkTelex, "日本語", "日本語"},
        RegressionVector{"Greek UTF-8", UkTelex, "Ελληνικά", "Ελληνικά"},
        RegressionVector{"Cyrillic UTF-8", UkTelex, "Привет", "Привет"},
        RegressionVector{"extended Latin UTF-8", UkTelex,
                         "élève Noël Łódź", "élève Noël Łódź"},
        RegressionVector{"emoji UTF-8", UkTelex, "hello 🌍🙂", "hello 🌍🙂"},
        RegressionVector{"mixed ASCII and UTF-8", UkTelex,
                         "ID 日本語 Ελληνικά 123",
                         "ID 日本語 Ελληνικά 123"},
        RegressionVector{"Unicode resets composition context", UkTelex,
                         "aw日本s", "ă日本s"},
        RegressionVector{"Unicode URL", UkTelex,
                         "https://例え.テスト/путь",
                         "https://例え.テスト/путь"},
        RegressionVector{"Unicode email", UkTelex,
                         "δοκιμή@demo.net", "δοκιμή@demo.net"},
    };
    context.runVectors(engine, vectors);
}

} // namespace unilume::test
