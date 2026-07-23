// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_assertions.h"

#include "utf8_validation.h"

#include <iostream>

namespace unilume::integration::test {

void Assertions::equal(std::string_view name,
                       std::string_view actual,
                       std::string_view expected)
{
    if (actual == expected) {
        return;
    }
    std::cerr << "FAIL " << name << ": expected [" << expected
              << "], got [" << actual << "]\n";
    ++failures_;
}

void Assertions::equal(std::string_view name,
                       std::uint64_t actual,
                       std::uint64_t expected)
{
    if (actual == expected) {
        return;
    }
    std::cerr << "FAIL " << name << ": expected " << expected
              << ", got " << actual << '\n';
    ++failures_;
}

void Assertions::truth(std::string_view name, bool value)
{
    if (!value) {
        std::cerr << "FAIL " << name << '\n';
        ++failures_;
    }
}

int Assertions::failures() const
{
    return failures_;
}

bool isValidUtf8(std::string_view text)
{
    return core::isValidUtf8(text);
}

} // namespace unilume::integration::test
