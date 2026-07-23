// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string_view>

namespace unilume::core {

enum class KeyKind {
    text,
    backspace,
    reset,
    navigation,
};

struct KeyInput {
    KeyKind kind{KeyKind::text};
    std::string_view text;
    bool shift_pressed{};
    bool caps_lock_on{};
    bool has_control_modifier{};
};

} // namespace unilume::core
