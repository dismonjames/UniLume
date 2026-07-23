// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "test_support/engine_fixture.h"

#include <span>
#include <string_view>

namespace unilume::test {

struct RegressionVector {
    std::string_view name;
    UkInputMethod method;
    std::string_view keys;
    std::string_view expected;
};

class TestContext {
public:
    void expectEqual(std::string_view name,
                     std::string_view actual,
                     std::string_view expected);
    void expectValidUtf8(std::string_view name, std::string_view text);
    void runVectors(EngineFixture &engine,
                    std::span<const RegressionVector> vectors);

    [[nodiscard]] int failures() const;

private:
    int failures_{};
};

} // namespace unilume::test
