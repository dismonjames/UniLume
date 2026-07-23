// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <string_view>

namespace unilume::integration::test {

class Assertions {
public:
    void equal(std::string_view name,
               std::string_view actual,
               std::string_view expected);
    void equal(std::string_view name,
               std::uint64_t actual,
               std::uint64_t expected);
    void truth(std::string_view name, bool value);
    [[nodiscard]] int failures() const;

private:
    int failures_{};
};

bool isValidUtf8(std::string_view text);

} // namespace unilume::integration::test
