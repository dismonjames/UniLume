// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <string_view>

namespace unilume::platform {

enum class ReplacementStatus {
    completed,
    pending,
    failed,
};

class ReplacementBackend {
public:
    virtual ~ReplacementBackend() = default;

    [[nodiscard]] virtual bool canReplace(
        std::int32_t delete_before_cursor) const = 0;
    virtual ReplacementStatus requestReplacement(
        std::uint64_t sequence_id,
        std::int32_t delete_before_cursor,
        std::string_view commit_text) = 0;
    virtual bool cancel(std::uint64_t sequence_id) = 0;
};

} // namespace unilume::platform
