// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

#include <array>
#include <string>
#include <string_view>

namespace unilume::test {

void runUnicodeInvalidInputTests(EngineFixture &engine, TestContext &context)
{
    static const std::array<std::string, 6> inputs{
        std::string{"\x80", 1},
        std::string{"\xc2", 1},
        std::string{"\xe2\x82", 2},
        std::string{"\xc0\xaf", 2},
        std::string{"\xed\xa0\x80", 3},
        std::string{"\xf5\x80\x80\x80", 4},
    };
    static constexpr std::array names{
        std::string_view{"lone continuation byte"},
        std::string_view{"truncated two-byte sequence"},
        std::string_view{"truncated three-byte sequence"},
        std::string_view{"overlong sequence"},
        std::string_view{"UTF-8 encoded surrogate"},
        std::string_view{"code point above Unicode range"},
    };

    for (std::size_t index = 0; index < inputs.size(); ++index) {
        const std::string actual = engine.type(inputs[index], UkTelex);
        context.expectEqual(names[index], actual, inputs[index]);
    }
}

} // namespace unilume::test
