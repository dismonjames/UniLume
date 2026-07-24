// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "replacement_backend.h"

#include <cstddef>
#include <cstdint>
#include <fcitx/inputcontext.h>

namespace unilume::fcitx5 {

struct ReplacementObservation {
    std::int32_t delete_before_cursor{};
    std::size_t commit_bytes{};
    bool surrounding_available{};
    bool cursor_valid{};
};

class FcitxReplacementBackend final
    : public platform::ReplacementBackend {
public:
    explicit FcitxReplacementBackend(fcitx::InputContext &input_context);

    [[nodiscard]] bool supportsDirectReplacement() const;
    [[nodiscard]] bool canReplace(
        std::int32_t delete_before_cursor) const override;
    platform::ReplacementStatus requestReplacement(
        std::uint64_t sequence_id,
        std::int32_t delete_before_cursor,
        std::string_view commit_text) override;
    bool cancel(std::uint64_t sequence_id) override;

    void reset();
    [[nodiscard]] const ReplacementObservation &lastObservation() const;

private:
    static std::size_t utf8Characters(std::string_view text);

    fcitx::InputContext &input_context_;
    std::uint64_t last_sequence_id_{};
    std::size_t committed_characters_{};
    mutable ReplacementObservation observation_;
};

} // namespace unilume::fcitx5
