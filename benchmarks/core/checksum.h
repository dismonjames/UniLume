// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <string_view>

namespace unilume::benchmark {

class Checksum {
public:
    void add(std::string_view bytes);
    void add(std::uint64_t value);
    [[nodiscard]] std::uint64_t value() const;

private:
    std::uint64_t value_{14695981039346656037ULL};
};

} // namespace unilume::benchmark
